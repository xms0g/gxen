#include "forwardRenderer.h"
#include <iostream>
#include <numeric>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../lightSystem.h"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../../mesh/mesh.h"
#include "../../config/config.hpp"
#include "../../core/camera.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../ECS/components/shader.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/instance.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "../../math/utils.hpp"

ForwardRenderer::ForwardRenderer() {
	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mSceneBuffer->withTextureMultisampled().withRenderBufferDepthStencilMultisampled().checkStatus();
	mSceneBuffer->unbind();

	mIntermediateBuffer = std::make_unique<FrameBuffer>(mSceneBuffer->width(), mSceneBuffer->height());
	mIntermediateBuffer->withTexture().withRenderBufferDepthStencil().checkStatus();
	mIntermediateBuffer->unbind();

	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);
#define MAX_DIR_LIGHTS  1
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  4
	int totalLightBufferSize =
			MAX_DIR_LIGHTS * sizeof(DirectionalLightComponent) +
			MAX_POINT_LIGHTS * sizeof(PointLightComponent) +
			MAX_SPOT_LIGHTS * sizeof(SpotLightComponent) + sizeof(glm::ivec4);

	mLightUBO = std::make_unique<UniformBuffer>(totalLightBufferSize, 1);
	glGenBuffers(1, &mStaticInstanceVBO);
	glGenBuffers(1, &mDynamicInstanceVBO);
}

void ForwardRenderer::configure(const Camera& camera) const {
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& mat = entity.getComponent<MaterialComponent>();

		mCameraUBO->configure(shader->ID(), 0, "CameraBlock");
		mLightUBO->configure(shader->ID(), 1, "LightBlock");

		if (mat.flags & Instanced) {
			prepareInstanceData(entity, mat.flags);
		}
	}

	const glm::mat4 projectionMat = glm::perspective(glm::radians(camera.zoom()),
	                                                 static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
	                                                 ZNEAR, ZFAR);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void ForwardRenderer::updateBuffers(const Camera& camera) const {
	updateCameraUBO(camera);
	updateLightUBO();
}

void ForwardRenderer::batchEntities(const Camera& camera) {
	for (const auto& entity: getSystemEntities()) {
		batchEntities(entity, camera);
	}
}

void ForwardRenderer::opaquePass() {
	for (const auto& entity: mOpaqueEntities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		shader->activate();
		opaquePass(entity, *shader);
	}
	mOpaqueEntities.clear();
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
		const auto& mc = entity.getComponent<MeshComponent>();
		const auto& ic = entity.getComponent<InstanceComponent>();

		shader->activate();
		materialPass(entity, *shader);

		for (const auto& mesh: *mc.meshes) {
			glBindVertexArray(mesh.VAO());
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices().size()),
			                        GL_UNSIGNED_INT, 0, ic.instancedCount);
		}
	}

	mInstancedEntities.clear();
}

void ForwardRenderer::transparentInstancedPass(const Camera& camera) {
	glDepthMask(GL_FALSE);
	for (auto& entity: mTransparentInstancedEntities) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& mc = entity.getComponent<MeshComponent>();
		const auto& ic = entity.getComponent<InstanceComponent>();
		const auto& mat = entity.getComponent<MaterialComponent>();

		auto positions = *ic.positions;

		std::sort(positions.begin(), positions.end(),
		          [&](const glm::vec3& a, const glm::vec3& b) {
			          const float da = glm::length2(camera.position() - a);
			          const float db = glm::length2(camera.position() - b);
			          return da > db; // back to front
		          });

		prepareInstanceData(entity, mat.flags);

		shader->activate();
		materialPass(entity, *shader);

		for (const auto& mesh: *mc.meshes) {
			glBindVertexArray(mesh.VAO());
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices().size()),
			                        GL_UNSIGNED_INT, 0, ic.instancedCount);
		}
	}

	glDepthMask(GL_TRUE);
	mTransparentInstancedEntities.clear();
}

void ForwardRenderer::beginSceneRender() const {
	mSceneBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::endSceneRender() const {
	mSceneBuffer->unbind();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mSceneBuffer->texture());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mIntermediateBuffer->texture());
	glBlitFramebuffer(0, 0, mSceneBuffer->width(), mSceneBuffer->height(), 0, 0, mSceneBuffer->width(),
	                  mSceneBuffer->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
		const auto& tc = entity.getComponent<TransformComponent>();

		if (mat.flags & Transparent) {
			float distance = glm::length(camera.position() - tc.position);
			mTransparentEntities.emplace_back(distance, entity);
		} else
			mOpaqueEntities.push_back(entity);
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

void ForwardRenderer::updateLightUBO() const {
	mLightUBO->bind();
	int offset = 0;
	// Directional lights
	const auto& dirLights = mLightSystem->getDirLights();
	for (size_t i = 0; i < dirLights.size(); i++) {
		mLightUBO->setData(dirLights[i], sizeof(DirectionalLightComponent),
		                   offset + i * sizeof(DirectionalLightComponent));
	}

	offset += MAX_DIR_LIGHTS * sizeof(DirectionalLightComponent);

	// Point lights
	const auto& pointLights = mLightSystem->getPointLights();
	for (size_t i = 0; i < pointLights.size(); i++) {
		mLightUBO->setData(pointLights[i], sizeof(PointLightComponent), offset + i * sizeof(PointLightComponent));
	}

	offset += MAX_POINT_LIGHTS * sizeof(PointLightComponent);

	// Spot Lights
	const auto& spotLights = mLightSystem->getSpotLights();
	for (size_t i = 0; i < spotLights.size(); i++) {
		mLightUBO->setData(spotLights[i], sizeof(SpotLightComponent), offset + i * sizeof(SpotLightComponent));
	}

	offset += MAX_SPOT_LIGHTS * sizeof(SpotLightComponent);

	auto lightCount = glm::ivec4(dirLights.size(), pointLights.size(), spotLights.size(), 0);
	mLightUBO->setData(glm::value_ptr(lightCount), sizeof(glm::ivec4), offset);

	mLightUBO->unbind();
}

void ForwardRenderer::prepareInstanceData(const Entity& entity, const uint32_t flags) const {
	const auto& tc = entity.getComponent<TransformComponent>();
	const auto& ic = entity.getComponent<InstanceComponent>();

	std::vector<InstanceData> gpuData;
	gpuData.reserve(ic.positions->size());

	for (auto& pos: *ic.positions) {
		auto [model, normal] = math::computeModelMatrices(pos, tc.rotation, tc.scale);
		gpuData.emplace_back(model, normal);
	}

	GLuint instanceVBO;
	if (flags & Transparent) {
		instanceVBO = mDynamicInstanceVBO;
		glBindBuffer(GL_ARRAY_BUFFER, mDynamicInstanceVBO);
		glBufferData(GL_ARRAY_BUFFER, gpuData.size() * sizeof(InstanceData), &gpuData[0],
		             GL_DYNAMIC_DRAW);
	} else {
		instanceVBO = mStaticInstanceVBO;
		glBindBuffer(GL_ARRAY_BUFFER, mStaticInstanceVBO);
		glBufferData(GL_ARRAY_BUFFER, gpuData.size() * sizeof(InstanceData), &gpuData[0],
		             GL_STATIC_DRAW);
	}

	const auto& mc = entity.getComponent<MeshComponent>();
	for (auto mesh: *mc.meshes) {
		mesh.enableInstanceAttributes(instanceVBO);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
