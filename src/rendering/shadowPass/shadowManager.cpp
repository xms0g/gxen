#include "shadowManager.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"

ShadowManager::ShadowManager() {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
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

	for (auto& light: lights.getSpotLights()) {
		mPerspectiveShadowPass->render(entities, light->direction, light->position, light->cutOff.y);
	}

	mShadowData.shadowMap = !lights.getDirLights().empty()
		                        ? mDirShadowPass->getShadowMap()
		                        : mPerspectiveShadowPass->getShadowMap();
	mShadowData.shadowCubemap = mOmnidirShadowPass->getShadowMap();
	mShadowData.lightSpaceMatrix = !lights.getDirLights().empty()
		                               ? mDirShadowPass->getLightSpaceMatrix()
		                               : mPerspectiveShadowPass->getLightSpaceMatrix();
	mShadowData.omniFarPlane = SHADOW_OMNIDIRECTIONAL_FAR;
}
