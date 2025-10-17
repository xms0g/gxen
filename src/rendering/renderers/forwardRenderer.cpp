#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../lightSystem.h"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../buffers/frameBuffer.h"
#include "../buffers/uniformBuffer.h"
#include "../../mesh/mesh.h"
#include "../../config/config.hpp"
#include "../../core/camera.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../ECS/components/shader.hpp"
#include "../../ECS/components/instance.hpp"
#include "../../math/utils.hpp"

ForwardRenderer::ForwardRenderer() {
	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mSceneBuffer->withTextureMultisampled(MULTISAMPLED_COUNT)
			.withRenderBufferDepthStencilMultisampled(MULTISAMPLED_COUNT)
			.checkStatus();
	mSceneBuffer->unbind();

	mIntermediateBuffer = std::make_unique<FrameBuffer>(mSceneBuffer->width(), mSceneBuffer->height());
	mIntermediateBuffer->withTexture()
			.withRenderBufferDepthStencil()
			.checkStatus();
	mIntermediateBuffer->unbind();

	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);

	glGenBuffers(1, &mStaticInstanceVBO.buffer);
	glGenBuffers(1, &mDynamicInstanceVBO.buffer);
}

ForwardRenderer::~ForwardRenderer() = default;

[[nodiscard]] uint32_t ForwardRenderer::getSceneTexture() const { return mSceneBuffer->texture(); }
[[nodiscard]] uint32_t ForwardRenderer::getIntermediateTexture() const { return mIntermediateBuffer->texture(); }
[[nodiscard]] uint32_t ForwardRenderer::getSceneWidth() const { return mSceneBuffer->width(); }
[[nodiscard]] uint32_t ForwardRenderer::getSceneHeight() const { return mSceneBuffer->height(); }

void ForwardRenderer::configure(const Camera& camera, const UniformBuffer& lightUBO) {
	size_t requiredOpaqueGPUBufferSize = 0, requiredTransparentGPUBufferSize = 0;

	// Calculate required GPU buffer sizes of opaque and transparent objects
	for (const auto& entity: getSystemEntities()) {
		const auto& mat = entity.getComponent<MaterialComponent>();

		if (mat.flags & Instanced) {
			const auto& ic = entity.getComponent<InstanceComponent>();

			if (mat.flags & Transparent) {
				requiredTransparentGPUBufferSize += ic.positions->size() * sizeof(InstanceData);
			} else {
				requiredOpaqueGPUBufferSize += ic.positions->size() * sizeof(InstanceData);
			}
		}
	}

	if (requiredOpaqueGPUBufferSize > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, mStaticInstanceVBO.buffer);
		glBufferData(GL_ARRAY_BUFFER, requiredOpaqueGPUBufferSize, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (requiredTransparentGPUBufferSize > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, mDynamicInstanceVBO.buffer);
		glBufferData(GL_ARRAY_BUFFER, requiredTransparentGPUBufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& mat = entity.getComponent<MaterialComponent>();

		mCameraUBO->configure(shader->ID(), 0, "CameraBlock");
		lightUBO.configure(shader->ID(), 1, "LightBlock");

		if (mat.flags & Instanced) {
			const auto& ic = entity.getComponent<InstanceComponent>();
			const size_t instanceSize = ic.positions->size() * sizeof(InstanceData);

			prepareInstanceData(entity, *ic.positions, instanceSize, mat.flags);
		}
	}
	mStaticInstanceVBO.offset = 0;
	mDynamicInstanceVBO.offset = 0;

	const glm::mat4 projectionMat = glm::perspective(
		glm::radians(camera.zoom()),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		ZNEAR, ZFAR);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void ForwardRenderer::updateBuffers(const Camera& camera) const {
	updateCameraUBO(camera);
}

void ForwardRenderer::batchEntities(const Camera& camera) {
	for (const auto& entity: getSystemEntities()) {
		batchEntities(entity, camera);
	}
}

void ForwardRenderer::opaquePass(const std::array<uint32_t, 3>& shadowMaps) {
	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[0]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMaps[1]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 2);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[2]);

	for (auto& [shader, entities]: mOpaqueBatches) {
		shader->activate();

		for (const auto& entity: entities) {
			shader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
			shader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
			shader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);
			opaquePass(entity, *shader);
		}
	}

	mOpaqueBatches.clear();
}

void ForwardRenderer::transparentPass() {
	if (mTransparentEntities.empty()) return;

	std::sort(mTransparentEntities.begin(), mTransparentEntities.end(),
	          [](const auto& a, const auto& b) { return a.first > b.first; });

	glDepthMask(GL_FALSE);
	for (auto& [dist, entity]: mTransparentEntities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		opaquePass(entity, *shader);
	}
	glDepthMask(GL_TRUE);
	mTransparentEntities.clear();
}

void ForwardRenderer::instancedPass() {
	for (const auto& entity: mInstancedEntities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& ic = entity.getComponent<InstanceComponent>();

		shader->activate();

		materialPass(entity, *shader);

		for (const auto& mesh: *entity.getComponent<MeshComponent>().meshes) {
			glBindVertexArray(mesh.VAO());
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<uint32_t>(mesh.indices().size()),
			                        GL_UNSIGNED_INT, 0, ic.positions->size());
		}
	}
	if (!glIsEnabled(GL_CULL_FACE))
		glEnable(GL_CULL_FACE);

	mInstancedEntities.clear();
}

void ForwardRenderer::transparentInstancedPass(const Camera& camera) {
	glDepthMask(GL_FALSE);
	for (auto& entity: mTransparentInstancedEntities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& ic = entity.getComponent<InstanceComponent>();
		const auto& mat = entity.getComponent<MaterialComponent>();

		auto positions = *ic.positions;

		std::sort(positions.begin(), positions.end(),
		          [&](const glm::vec3& a, const glm::vec3& b) {
			          const float da = glm::length2(camera.position() - a);
			          const float db = glm::length2(camera.position() - b);
			          return da > db; // back to front
		          });

		prepareInstanceData(entity, positions, positions.size() * sizeof(InstanceData), mat.flags);

		shader->activate();
		materialPass(entity, *shader);

		for (const auto& mesh: *entity.getComponent<MeshComponent>().meshes) {
			glBindVertexArray(mesh.VAO());
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<uint32_t>(mesh.indices().size()),
			                        GL_UNSIGNED_INT, 0, positions.size());
		}
	}

	glDepthMask(GL_TRUE);
	if (!glIsEnabled(GL_CULL_FACE))
		glEnable(GL_CULL_FACE);
	mTransparentInstancedEntities.clear();
	mDynamicInstanceVBO.offset = 0;
}

void ForwardRenderer::beginSceneRender() const {
	mSceneBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::endSceneRender() const {
	mSceneBuffer->unbind();
	mSceneBuffer->bindForRead();
	mIntermediateBuffer->bindForDraw();
	glBlitFramebuffer(0, 0, mSceneBuffer->width(), mSceneBuffer->height(), 0, 0, mIntermediateBuffer->width(),
	                  mIntermediateBuffer->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderer::batchEntities(const Entity& entity, const Camera& camera) {
	const auto& mat = entity.getComponent<MaterialComponent>();

	if (mat.flags & Instanced) {
		if (mat.flags & Transparent) {
			mTransparentInstancedEntities.push_back(entity);
		} else
			mInstancedEntities.push_back(entity);
	} else {
		if (mat.flags & Transparent) {
			const auto& tc = entity.getComponent<TransformComponent>();
			float distance = glm::length2(camera.position() - tc.position);
			mTransparentEntities.emplace_back(distance, entity);
		} else {
			auto& shader = *entity.getComponent<ShaderComponent>().shader;
			mOpaqueBatches[&shader].push_back(entity);
		}
	}
}

void ForwardRenderer::updateCameraUBO(const Camera& camera) const {
	auto view = camera.viewMatrix();
	auto viewPos = glm::vec4(camera.position(), 1.0);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(view), sizeof(glm::mat4), 0);
	mCameraUBO->setData(glm::value_ptr(viewPos), sizeof(glm::vec4), 2 * sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void ForwardRenderer::prepareInstanceData(const Entity& entity, const std::vector<glm::vec3>& positions,
                                          const size_t instanceSize, const uint32_t flags) {
	const auto& tc = entity.getComponent<TransformComponent>();

	std::vector<InstanceData> gpuData;
	gpuData.reserve(positions.size());

	for (auto& pos: positions) {
		auto [model, normal] = math::computeModelMatrices(pos, tc.rotation, tc.scale);
		gpuData.emplace_back(model, normal);
	}

	if (gpuData.empty()) return;

	const GLuint vbo = flags & Transparent ? mDynamicInstanceVBO.buffer : mStaticInstanceVBO.buffer;
	const size_t offset = flags & Transparent ? mDynamicInstanceVBO.offset : mStaticInstanceVBO.offset;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, instanceSize, gpuData.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (const auto& mesh: *entity.getComponent<MeshComponent>().meshes) {
		mesh.enableInstanceAttributes(vbo, offset);
	}

	if (flags & Transparent) {
		mDynamicInstanceVBO.offset += instanceSize;
	} else {
		mStaticInstanceVBO.offset += instanceSize;
	}
}
