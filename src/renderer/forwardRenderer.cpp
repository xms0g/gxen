#include "forwardRenderer.h"
#include <iostream>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "lightSystem.h"
#include "shader.h"
#include "../mesh/mesh.h"
#include "../config/config.hpp"
#include "../core/camera.h"
#include "../ECS/registry.h"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/directionalLight.hpp"
#include "../ECS/components/pointLight.hpp"
#include "../ECS/components/spotLight.hpp"

ForwardRenderer::ForwardRenderer() {
	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mSceneBuffer->withTexture().withRenderBufferDepthStencil().checkStatus();
	mSceneBuffer->unbind();

	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);
#define MAX_DIR_LIGHTS  1
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  4
	int totalLightBufferSize =
			MAX_DIR_LIGHTS * sizeof(DirectionalLightComponent) +
			MAX_POINT_LIGHTS * sizeof(PointLightComponent) +
			MAX_SPOT_LIGHTS * sizeof(SpotLightComponent) + sizeof(glm::ivec4);

	mLightUBO = std::make_unique<UniformBuffer>(totalLightBufferSize, 1);
}

void ForwardRenderer::configure(const Camera& camera) const {
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		mCameraUBO->configure(shader->ID(), 0, "CameraBlock");
		mLightUBO->configure(shader->ID(), 1, "LightBlock");
	}

	const glm::mat4 projectionMat = glm::perspective(glm::radians(camera.zoom()),
	                                                 static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
	                                                 ZNEAR, ZFAR);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void ForwardRenderer::render(const Camera& camera) {
	updateCameraUBO(camera);
	updateLightUBO();

	TransEntityBucket transparentEntities;
	for (const auto& entity: getSystemEntities()) {
		if (collectTransparentEntities(entity, camera, transparentEntities))
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		shader->activate();
		opaquePass(entity, *shader);
	}

	transparentPass(transparentEntities);
}

void ForwardRenderer::beginSceneRender() const {
	mSceneBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::endSceneRender() const {
	mSceneBuffer->unbind();
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