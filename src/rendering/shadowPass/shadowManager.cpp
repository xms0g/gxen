#include "shadowManager.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../../config/config.hpp"

ShadowManager::ShadowManager() {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
}

ShadowManager::~ShadowManager() = default;

ShadowData& ShadowManager::getShadowData() {
	return mShadowData;
}

void ShadowManager::render(const std::vector<Entity>& entities, const LightSystem& lights) {
	mDirShadowPass->render(entities, lights);
	mOmnidirShadowPass->render(entities, lights);

	mShadowData.shadowMap = mDirShadowPass->getShadowMap();
	mShadowData.shadowCubemap = mOmnidirShadowPass->getShadowMap();
	mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	mShadowData.omniFarPlane = SHADOW_OMNIDIRECTIONAL_FAR;
}
