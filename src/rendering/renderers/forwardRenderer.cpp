#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "renderCommon.h"
#include "../renderItem.hpp"
#include "../instanceGroup.hpp"
#include "../shader.h"
#include "../material/material.hpp"
#include "../mesh/mesh.h"
#include "../../config/config.hpp"
#include "../../ECS/registry.h"
#include "../../ECS/components/bv.hpp"
#include "../../math/matrix.hpp"

ForwardRenderer::ForwardRenderer() {
	glGenBuffers(1, &mOpaqueInstanceVBO.buffer);
	glGenBuffers(1, &mTransparentInstanceVBO.buffer);
}

ForwardRenderer::~ForwardRenderer() = default;

void ForwardRenderer::configure(const std::vector<InstanceGroup>& opaqueInstancedGroup,
                                const std::vector<InstanceGroup>& blendInstancedGroup) {
	prepareInstanceBuffer(opaqueInstancedGroup, mOpaqueInstanceVBO);
	prepareInstanceData(opaqueInstancedGroup, mOpaqueInstanceVBO);

	prepareInstanceBuffer(blendInstancedGroup, mTransparentInstanceVBO);
	prepareInstanceData(blendInstancedGroup, mTransparentInstanceVBO);
}

void ForwardRenderer::opaquePass(const std::unordered_map<const Shader*, std::vector<RenderItem> >& renderItems,
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

void ForwardRenderer::transparentPass(const std::unordered_map<const Shader*, std::vector<RenderItem> >& renderItems) const {
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

void ForwardRenderer::opaqueInstancedPass(const std::vector<InstanceGroup>& instancedGroup,
                                          const std::array<uint32_t, 3>& shadowMaps) {
	if (instancedGroup.empty()) return;

	RenderCommon::bindShadowMaps(shadowMaps);
	instancedPass(instancedGroup);
}

void ForwardRenderer::transparentInstancedPass(const std::vector<InstanceGroup>& instancedGroup) {
	if (instancedGroup.empty()) return;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	instancedPass(instancedGroup);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ForwardRenderer::instancedPass(const std::vector<InstanceGroup>& instancedGroup) {
	for (const auto& [entity, transforms, materials]: instancedGroup) {
		const size_t count = transforms->size() / 9;

		for (const auto& [material, shader, meshes]: materials) {
			shader->activate();
			shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
			shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
			shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);

			RenderCommon::setupMaterial(*entity, *shader);
			RenderCommon::bindTextures(material->textures, *shader);

			for (const auto& mesh: meshes) {
				glBindVertexArray(mesh->VAO());
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<int32_t>(mesh->indices().size()),
										GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(count));
			}

			RenderCommon::unbindTextures(material->textures);
		}
	}
}

void ForwardRenderer::prepareInstanceData(const std::vector<InstanceGroup>& instancedGroup, const InstanceVBO& vbo) {
	for (const auto& [entity, transforms, materials]: instancedGroup) {
		std::vector<InstanceData> gpuData;
		gpuData.reserve(transforms->size() / 9);

		auto transform = *transforms;
		for (int i = 0; i < transform.size(); i += 9) {
			glm::vec3 pos{transform[i], transform[i + 1], transform[i + 2]};
			glm::vec3 rot{transform[i + 3], transform[i + 4], transform[i + 5]};
			glm::vec3 scale{transform[i + 6], transform[i + 7], transform[i + 8]};

			const glm::mat4 model = math::computeModelMatrix(pos, rot, scale);
			const glm::mat3 normal = math::computeNormalMatrix(model);
			gpuData.emplace_back(model, normal);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
		glBufferSubData(GL_ARRAY_BUFFER, vbo.offset, static_cast<long>(gpuData.size() * sizeof(InstanceData)), gpuData.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void ForwardRenderer::prepareInstanceBuffer(const std::vector<InstanceGroup>& instancedGroup, InstanceVBO& vbo) {
	if (instancedGroup.empty()) return;

	size_t requiredGPUBufferSize = 0;

	for (const auto& [entity, transforms, materials]: instancedGroup) {
		for (const auto& material: materials) {
			for (const auto& mesh: material.meshes) {
				mesh->enableInstanceAttributes(vbo.buffer, vbo.offset);
			}
		}

		const size_t count = transforms->size() / 9;
		const size_t instanceSize = count * sizeof(InstanceData);
		requiredGPUBufferSize += instanceSize;
		vbo.offset += static_cast<int>(instanceSize);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long>(requiredGPUBufferSize), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vbo.offset = 0;
}
