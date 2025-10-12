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
	mShadowData.dirShadows.clear();
	mShadowData.pointShadows.clear();

	for (auto& light: lights.getDirLights()) {
		mDirShadowPass->render(entities, light->direction);
		mShadowData.dirShadows.emplace_back(mDirShadowPass->getShadowMap(), mDirShadowPass->getLightSpaceMatrix());
	}

	for (auto& light: lights.getPointLights()) {
		mOmnidirShadowPass->render(entities, light->position);
		mShadowData.pointShadows.emplace_back(mOmnidirShadowPass->getShadowMap(), SHADOW_OMNIDIRECTIONAL_FAR);
	}
}
