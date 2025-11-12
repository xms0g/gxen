#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "renderCommon.h"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../../mesh/mesh.h"
#include "../../config/config.hpp"
#include "../../ECS/registry.h"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../ECS/components/shader.hpp"
#include "../../ECS/components/instance.hpp"
#include "../../math/matrix.hpp"

ForwardRenderer::ForwardRenderer() {
	glGenBuffers(1, &mOpaqueInstanceVBO.buffer);
	glGenBuffers(1, &mTransparentInstanceVBO.buffer);
}

ForwardRenderer::~ForwardRenderer() = default;

void ForwardRenderer::configure(const std::vector<Entity>& opaqueInstancedEntities,
                                const std::vector<Entity>& transparentInstancedEntities) {
	prepareInstanceBuffer(opaqueInstancedEntities, mOpaqueInstanceVBO);
	prepareInstanceBuffer(transparentInstancedEntities, mTransparentInstanceVBO);
}

void ForwardRenderer::opaquePass(const std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches,
                                 const std::array<uint32_t, 3>& shadowMaps) const {
	for (uint32_t i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + i);
		glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
	}

	for (const auto& [shader, entities]: opaqueBatches) {
		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		for (const auto& entity: entities) {
			if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
				continue;

			RenderCommon::setupTransform(entity, *shader);
			RenderCommon::setupMaterial(entity, *shader);
			RenderCommon::drawMeshes(entity, *shader);
		}
	}
}

void ForwardRenderer::transparentPass(TransEntityBucket& entities) const {
	if (entities.empty()) return;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& [dist, entity]: entities) {
		if (!entity.getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		RenderCommon::setupTransform(entity, *shader);
		RenderCommon::setupMaterial(entity, *shader);
		RenderCommon::drawMeshes(entity, *shader);
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ForwardRenderer::instancedPass(const std::vector<Entity>& entities,
                                    const std::array<uint32_t, 3>& shadowMaps) const {
	for (uint32_t i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + i);
		glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
	}

	for (const auto& entity: entities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& texturesByMatID = entity.getComponent<MaterialComponent>().textures;
		const size_t instanceCount = entity.getComponent<InstanceComponent>().positions->size();

		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		RenderCommon::setupMaterial(entity, *shader);

		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			RenderCommon::bindTextures(matID, texturesByMatID, *shader);
			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh.VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()),
				                        GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(instanceCount));
			}
			RenderCommon::unbindTextures(matID, texturesByMatID);
		}
	}
}

void ForwardRenderer::transparentInstancedPass(const std::vector<Entity>& entities) const {
	if (entities.empty()) return;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto& entity: entities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& texturesByMatID = entity.getComponent<MaterialComponent>().textures;
		const size_t instanceCount = entity.getComponent<InstanceComponent>().positions->size();

		shader->activate();

		RenderCommon::setupMaterial(entity, *shader);

		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			RenderCommon::bindTextures(matID, texturesByMatID, *shader);
			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh.VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()),
				                        GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(instanceCount));
			}
			RenderCommon::unbindTextures(matID, texturesByMatID);
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ForwardRenderer::prepareInstanceData(const Entity& entity,
                                          const std::vector<glm::vec3>& positions,
                                          const size_t instanceSize,
                                          const uint32_t flags) {
	const auto& tc = entity.getComponent<TransformComponent>();

	std::vector<InstanceData> gpuData;
	gpuData.reserve(positions.size());

	for (auto& pos: positions) {
		const glm::mat4 model = math::computeModelMatrix(pos, tc.rotation, tc.scale);
		const glm::mat3 normal = math::computeNormalMatrix(model);
		gpuData.emplace_back(model, normal);
	}

	if (gpuData.empty()) return;

	const uint32_t vbo = flags & Transparent ? mTransparentInstanceVBO.buffer : mOpaqueInstanceVBO.buffer;
	const int offset = flags & Transparent ? mTransparentInstanceVBO.offset : mOpaqueInstanceVBO.offset;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, static_cast<long>(instanceSize), gpuData.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
		for (const auto& mesh: meshes) {
			mesh.enableInstanceAttributes(vbo, offset);
		}
	}

	if (flags & Transparent) {
		mTransparentInstanceVBO.offset += static_cast<int>(instanceSize);
	} else {
		mOpaqueInstanceVBO.offset += static_cast<int>(instanceSize);
	}
}

void ForwardRenderer::prepareInstanceBuffer(const std::vector<Entity>& entities, InstanceVBO& vbo) {
	size_t requiredGPUBufferSize = 0;

	for (const auto& entity: entities) {
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);
		requiredGPUBufferSize += instanceSize;
	}

	if (requiredGPUBufferSize > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<long>(requiredGPUBufferSize), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	for (const auto& entity: entities) {
		const auto& mat = entity.getComponent<MaterialComponent>();
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);

		prepareInstanceData(entity, *ic.positions, instanceSize, mat.flags);
	}

	vbo.offset = 0;
}
