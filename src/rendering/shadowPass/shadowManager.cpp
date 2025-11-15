#include "shadowManager.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../buffers/uniformBuffer.h"
#include "../buffers/frameBuffer.h"
#include "../renderers/forwardRenderer.h"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "glad/glad.h"

ShadowManager::ShadowManager() {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);

	mShadowUBO = std::make_unique<UniformBuffer>(sizeof(ShadowData), 2);

	mShadowMaps = {
		mDirShadowPass->getShadowMap(),
		mOmnidirShadowPass->getDepthTexture(),
		mPerspectiveShadowPass->getShadowMap()
	};
}

ShadowManager::~ShadowManager() = default;

const std::array<uint32_t, 3>& ShadowManager::getShadowMaps() const { return mShadowMaps; }
const UniformBuffer& ShadowManager::getShadowUBO() const { return *mShadowUBO; }

void ShadowManager::shadowPass(std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches,
                               const LightSystem& lights) {
	for (const auto& [shader, entities]: opaqueBatches) {
		for (const auto& light: lights.getDirLights()) {
			mDirShadowPass->render(entities, light->direction);
			mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
		}

		const auto& pointLights = lights.getPointLights();
		mOmnidirShadowPass->getDepthMap().bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		for (int i = 0; i < pointLights.size(); i++) {
			const auto& light = pointLights[i];
			if (!light->castShadow) continue;

			mOmnidirShadowPass->render(entities, light->position, i);
			mShadowData.omniFarPlanes[i] = SHADOW_OMNIDIRECTIONAL_FAR;
		}
		mOmnidirShadowPass->getDepthMap().unbind();
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		const auto& spotLights = lights.getSpotLights();
		for (int i = 0; i < spotLights.size(); i++) {
			const auto& light = spotLights[i];
			if (!light->castShadow) continue;

			mPerspectiveShadowPass->render(entities, light->direction, light->position, light->cutOff.y, i);
			mShadowData.persLightSpaceMatrix[i] = mPerspectiveShadowPass->getLightSpaceMatrix(i);
		}
	}

	mShadowUBO->bind();
	mShadowUBO->setData(&mShadowData, sizeof(ShadowData), 0);
	mShadowUBO->unbind();
}
