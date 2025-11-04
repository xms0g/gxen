#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../../mesh/mesh.h"
#include "../../config/config.hpp"
#include "../../core/camera.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../ECS/components/shader.hpp"
#include "../../ECS/components/instance.hpp"
#include "../../math/matrix.hpp"
#include "../../resourceManager/texture.h"

ForwardRenderer::ForwardRenderer() {
	glGenBuffers(1, &mStaticInstanceVBO.buffer);
	glGenBuffers(1, &mDynamicInstanceVBO.buffer);
}

ForwardRenderer::~ForwardRenderer() = default;

void ForwardRenderer::configure(const std::vector<Entity>& opaqueInstancedEntities,
                                const std::vector<Entity>& transparentInstancedEntities) {
	size_t requiredOpaqueGPUBufferSize = 0, requiredTransparentGPUBufferSize = 0;

	// Calculate required GPU buffer sizes of opaque and transparent objects
	for (const auto& entity: opaqueInstancedEntities) {
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);
		requiredOpaqueGPUBufferSize += instanceSize;
	}

	for (const auto& entity: transparentInstancedEntities) {
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);
		requiredTransparentGPUBufferSize += instanceSize;
	}

	if (requiredOpaqueGPUBufferSize > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, mStaticInstanceVBO.buffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<long>(requiredOpaqueGPUBufferSize), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (requiredTransparentGPUBufferSize > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, mDynamicInstanceVBO.buffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<long>(requiredTransparentGPUBufferSize), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	for (const auto& entity: opaqueInstancedEntities) {
		const auto& mat = entity.getComponent<MaterialComponent>();
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);

		prepareInstanceData(entity, *ic.positions, instanceSize, mat.flags);
	}

	mStaticInstanceVBO.offset = 0;
	mDynamicInstanceVBO.offset = 0;
}

void ForwardRenderer::opaquePass(const std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches,
                                 const std::array<uint32_t, 3>& shadowMaps) const {
	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[0]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMaps[1]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 2);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[2]);

	for (const auto& [shader, entities]: opaqueBatches) {
		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		for (const auto& entity: entities) {
			if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
				continue;

			opaquePass(entity, *shader);
		}
	}
}

void ForwardRenderer::transparentPass(TransEntityBucket& entities) const {
	if (entities.empty()) return;

	std::sort(entities.begin(), entities.end(),
	          [](const auto& a, const auto& b) { return a.first > b.first; });

	glDepthMask(GL_FALSE);
	for (const auto& [dist, entity]: entities) {
		if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		opaquePass(entity, *shader);
	}
	glDepthMask(GL_TRUE);
}

void ForwardRenderer::instancedPass(const std::vector<Entity>& entities,
                                    const std::array<uint32_t, 3>& shadowMaps) const {
	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[0]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMaps[1]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 2);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[2]);

	for (const auto& entity: entities) {
		if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& ic = entity.getComponent<InstanceComponent>();
		const auto& texturesByMatID = entity.getComponent<MaterialComponent>().textures;

		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		materialPass(entity, *shader);

		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			bindTextures(matID, texturesByMatID, *shader);
			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh.VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()),
				                        GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(ic.positions->size()));
			}
			unbindTextures(matID, texturesByMatID);
		}
	}
}

void ForwardRenderer::transparentInstancedPass(const std::vector<Entity>& entities, const Camera& camera) {
	glDepthMask(GL_FALSE);
	for (auto& entity: entities) {
		if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& ic = entity.getComponent<InstanceComponent>();
		const auto& mat = entity.getComponent<MaterialComponent>();
		const auto& texturesByMatID = mat.textures;

		auto& positions = *ic.positions;

		std::sort(
			positions.begin(), positions.end(),
			[&](const glm::vec3& a, const glm::vec3& b) {
				const float da = glm::length2(camera.position() - a);
				const float db = glm::length2(camera.position() - b);
				return da > db; // back to front
			});

		prepareInstanceData(entity, positions, positions.size() * sizeof(InstanceData), mat.flags);

		shader->activate();

		materialPass(entity, *shader);

		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			bindTextures(matID, texturesByMatID, *shader);
			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh.VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()),
				                        GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(positions.size()));
			}
			unbindTextures(matID, texturesByMatID);
		}
	}

	glDepthMask(GL_TRUE);
	mDynamicInstanceVBO.offset = 0;
}

void ForwardRenderer::opaquePass(const Entity& entity, const Shader& shader) const {
	geometryPass(entity, shader);
	materialPass(entity, shader);
	drawPass(entity, shader);
}

void ForwardRenderer::geometryPass(const Entity& entity, const Shader& shader) const {
	const auto& tc = entity.getComponent<TransformComponent>();

	const glm::mat4 model = math::computeModelMatrix(tc.position, tc.rotation, tc.scale);
	const glm::mat3 normal = math::computeNormalMatrix(model);

	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", normal);
}

void ForwardRenderer::materialPass(const Entity& entity, const Shader& shader) const {
	const auto& mtc = entity.getComponent<MaterialComponent>();

	if (mtc.flags & TwoSided) {
		glDisable(GL_CULL_FACE);
	}

	shader.setFloat("material.shininess", mtc.shininess);
	shader.setFloat("material.heightScale", mtc.heightScale);

	if (!mtc.textures) {
		shader.setVec3("material.color", mtc.color);
	}
}

void ForwardRenderer::drawPass(const Entity& entity, const Shader& shader) const {
	const auto& texturesByMatID = entity.getComponent<MaterialComponent>().textures;

	for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
		bindTextures(matID, texturesByMatID, shader);

		for (const auto& mesh: meshes) {
			glBindVertexArray(mesh.VAO());
			glDrawElements(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()), GL_UNSIGNED_INT, nullptr);
		}
		unbindTextures(matID, texturesByMatID);
	}
}

void ForwardRenderer::bindTextures(const uint32_t materialID,
                                   const TextureMap* texturesByMatID,
                                   const Shader& shader) const {
	if (texturesByMatID && !texturesByMatID->empty()) {
		bool hasNormalMap{false}, hasHeightMap{false};
		const auto& textures = texturesByMatID->at(materialID);

		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

			std::string name = textures[i].type;

			if (name == "texture_normal") {
				hasNormalMap = true;
			}

			if (name == "texture_height") {
				hasHeightMap = true;
			}

			// now set the sampler to the correct texture unit
			shader.setInt(std::string("material.").append(name), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		shader.setBool("material.hasNormalMap", hasNormalMap);
		shader.setBool("material.hasHeightMap", hasHeightMap);
	}
}

void ForwardRenderer::unbindTextures(const uint32_t materialID,
                                     const TextureMap* texturesByMatID) const {
	if (texturesByMatID && !texturesByMatID->empty()) {
		const auto& textures = texturesByMatID->at(materialID);

		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void ForwardRenderer::prepareInstanceData(const Entity& entity,
                                          const std::vector<glm::vec3>& positions,
                                          const size_t instanceSize,
                                          const uint32_t flags) {
	const auto& tc = entity.getComponent<TransformComponent>();

	std::vector<InstanceData> gpuData;
	gpuData.reserve(positions.size());

	for (auto& pos: positions) {
		const glm::mat4 model = math::computeModelMatrix(tc.position, tc.rotation, tc.scale);
		const glm::mat3 normal = math::computeNormalMatrix(model);
		gpuData.emplace_back(model, normal);
	}

	if (gpuData.empty()) return;

	const uint32_t vbo = flags & Transparent ? mDynamicInstanceVBO.buffer : mStaticInstanceVBO.buffer;
	const int offset = flags & Transparent ? mDynamicInstanceVBO.offset : mStaticInstanceVBO.offset;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, static_cast<long>(instanceSize), gpuData.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
		for (const auto& mesh: meshes) {
			mesh.enableInstanceAttributes(vbo, offset);
		}
	}

	if (flags & Transparent) {
		mDynamicInstanceVBO.offset += static_cast<int>(instanceSize);
	} else {
		mStaticInstanceVBO.offset += static_cast<int>(instanceSize);
	}
}
