#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "renderCommon.h"
#include "../renderItem.hpp"
#include "../shader.h"
#include "../material/material.hpp"
#include "../mesh/mesh.h"
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

void ForwardRenderer::opaquePass(const std::unordered_map<Shader*, std::vector<RenderItem>>& renderItems,
                                 const std::array<uint32_t, 3>& shadowMaps) const {
	if (renderItems.empty()) return;

	RenderCommon::bindShadowMaps(shadowMaps);

	for (const auto& [shader, items]: renderItems) {
		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		for (const auto& item: items) {
			if (!item.entity->getComponent<BoundingVolumeComponent>().isVisible)
				continue;

			RenderCommon::setupTransform(*item.entity, *shader);
			RenderCommon::setupMaterial(*item.entity, *shader);
			RenderCommon::drawMesh(item, *shader);
		}
	}
}

void ForwardRenderer::transparentPass(const std::unordered_map<Shader*, std::vector<RenderItem>>& renderItems) const {
	if (renderItems.empty()) return;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& [shader, items]: renderItems) {
		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		for (const auto& item: items) {
			if (!item.entity->getComponent<BoundingVolumeComponent>().isVisible)
				continue;

			RenderCommon::setupTransform(*item.entity, *shader);
			RenderCommon::setupMaterial(*item.entity, *shader);
			RenderCommon::drawMesh(item, *shader);
		}
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ForwardRenderer::opaqueInstancedPass(const std::vector<Entity>& entities,
                                          const std::array<uint32_t, 3>& shadowMaps) {
	if (entities.empty()) return;

	RenderCommon::bindShadowMaps(shadowMaps);
	instancedPass(entities, mOpaqueInstanceVBO);
}

void ForwardRenderer::transparentInstancedPass(const std::vector<Entity>& entities) {
	if (entities.empty()) return;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	instancedPass(entities, mTransparentInstanceVBO);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ForwardRenderer::instancedPass(const std::vector<Entity>& entities, InstanceVBO& vbo) {
	for (const auto& entity: entities) {
		const auto& ic = entity.getComponent<InstanceComponent>();
		const size_t instanceCount = ic.positions->size();
		const size_t instanceSize = instanceCount * sizeof(InstanceData);

		prepareInstanceData(entity, *ic.positions, instanceSize, vbo);
		vbo.offset += static_cast<int>(instanceSize);

		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& materials = entity.getComponent<MaterialComponent>().materials;

		shader->activate();
		shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
		shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
		shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

		RenderCommon::setupMaterial(entity, *shader);

		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			auto& material = materials->at(matID);
			RenderCommon::bindTextures(material.textures, *shader);
			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh.VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh.indices().size()),
										GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(instanceCount));
			}
			RenderCommon::unbindTextures(material.textures);
		}
	}
	vbo.offset = 0;
}

void ForwardRenderer::prepareInstanceData(const Entity& entity, const std::vector<glm::vec3>& positions,
                                          const size_t instanceSize, const InstanceVBO& vbo) {
	const auto& tc = entity.getComponent<TransformComponent>();

	std::vector<InstanceData> gpuData;
	gpuData.reserve(positions.size());

	for (auto& pos: positions) {
		const glm::mat4 model = math::computeModelMatrix(pos, tc.rotation, tc.scale);
		const glm::mat3 normal = math::computeNormalMatrix(model);
		gpuData.emplace_back(model, normal);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
	glBufferSubData(GL_ARRAY_BUFFER, vbo.offset, static_cast<long>(instanceSize), gpuData.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ForwardRenderer::prepareInstanceBuffer(const std::vector<Entity>& entities, InstanceVBO& vbo) {
	if (entities.empty()) return;

	size_t requiredGPUBufferSize = 0;

	for (const auto& entity: entities) {
		for (const auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
			for (const auto& mesh: meshes) {
				mesh.enableInstanceAttributes(vbo.buffer, vbo.offset);
			}
		}

		const size_t instanceSize = entity.getComponent<InstanceComponent>().positions->size() * sizeof(InstanceData);

		requiredGPUBufferSize += instanceSize;
		vbo.offset += static_cast<int>(instanceSize);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long>(requiredGPUBufferSize), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vbo.offset = 0;
}
