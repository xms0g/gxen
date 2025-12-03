#include "shadowManager.h"
#include "glad/glad.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../shader.h"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../lightSystem.h"
#include "../buffers/uniformBuffer.h"
#include "../buffers/frameBuffer.h"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"

ShadowManager::ShadowManager() {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);

	mShadowUBO = std::make_unique<UniformBuffer>(sizeof(ShadowData), 2);

	mShadowMaps = {
		mDirShadowPass->getDepthTexture(),
		mOmnidirShadowPass->getDepthTexture(),
		mPerspectiveShadowPass->getDepthTexture()
	};
}

ShadowManager::~ShadowManager() = default;

const std::array<uint32_t, 3>& ShadowManager::getShadowMaps() const { return mShadowMaps; }

const UniformBuffer* ShadowManager::getShadowUBO() const { return mShadowUBO.get(); }

void ShadowManager::shadowPass(const RenderContext& context) {
	directionalShadowPass(context);
	omnidirectionalShadowPass(context);
	perspectiveShadowPass(context);

	mShadowUBO->bind();
	mShadowUBO->setData(&mShadowData, sizeof(ShadowData), 0);
	mShadowUBO->unbind();
}

void ShadowManager::directionalShadowPass(const RenderContext& context) {
	for (const auto& light: context.lightSystem->getDirLights()) {
		mDirShadowPass->render(context.renderQueue->shadowCasterGroups, light->direction);
		mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	}
}

void ShadowManager::omnidirectionalShadowPass(const RenderContext& context) {
	mOmnidirShadowPass->getDepthMap().bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	const auto& lights = context.lightSystem->getPointLights();
	for (int i = 0; i < lights.size(); i++) {
		const auto& light = lights[i];
		if (!light->castShadow) continue;

		mOmnidirShadowPass->render(context.renderQueue->shadowCasterGroups, light->position, i);
		mShadowData.omniFarPlanes[i] = SHADOW_OMNIDIRECTIONAL_FAR;
	}

	mOmnidirShadowPass->getDepthMap().unbind();
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
}

void ShadowManager::perspectiveShadowPass(const RenderContext& context) {
	const auto& lights = context.lightSystem->getSpotLights();
	for (int i = 0; i < lights.size(); i++) {
		const auto& light = lights[i];
		if (!light->castShadow) continue;

		mPerspectiveShadowPass->render(context.renderQueue->shadowCasterGroups, light->direction, light->position, light->cutOff.y, i);
		mShadowData.persLightSpaceMatrix[i] = mPerspectiveShadowPass->getLightSpaceMatrix(i);
	}
}
