#include "shadowManager.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"

ShadowManager::ShadowManager() {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
}

ShadowManager::~ShadowManager() = default;

ShadowData& ShadowManager::getShadowData() {
	return mShadowData;
}

void ShadowManager::render(const std::vector<Entity>& entities, const LightSystem& lights) {
	for (auto& light: lights.getDirLights()) {
		mDirShadowPass->render(entities, light->direction);
	}

	for (auto& light: lights.getPointLights()) {
		mOmnidirShadowPass->render(entities, light->position);
	}

	mShadowData.shadowMap = mDirShadowPass->getShadowMap();
	mShadowData.shadowCubemap = mOmnidirShadowPass->getShadowMap();
	mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	mShadowData.omniFarPlane = SHADOW_OMNIDIRECTIONAL_FAR;
}
