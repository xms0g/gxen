#include "shadowSystem.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../renderFlags.hpp"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "../../ECS/components/material.hpp"

ShadowSystem::ShadowSystem() {
	RequireComponent<MaterialComponent>();
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
}

ShadowSystem::~ShadowSystem() = default;

ShadowData& ShadowSystem::getShadowData() {
	return mShadowData;
}

void ShadowSystem::render(const LightSystem& lights) {
	mEntities.clear();

	for (auto& entity: getSystemEntities()) {
		auto& mat = entity.getComponent<MaterialComponent>();

		if (mat.flags & Opaque) {
			mEntities.push_back(entity);}
	}

	for (auto& light: lights.getDirLights()) {
		mDirShadowPass->render(mEntities, light->direction);
	}

	for (auto& light: lights.getPointLights()) {
		mOmnidirShadowPass->render(mEntities, light->position);
	}

	for (auto& light: lights.getSpotLights()) {
		mPerspectiveShadowPass->render(mEntities, light->direction, light->position, light->cutOff.y);
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
