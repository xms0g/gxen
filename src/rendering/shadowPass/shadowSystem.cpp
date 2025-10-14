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

void ShadowSystem::shadowPass(const LightSystem& lights) {
	mEntities.clear();
	mShadowData.omnidirShadows.clear();
	mShadowData.omnidirShadows.reserve(lights.getPointLights().size());
	mShadowData.persShadows.clear();
	mShadowData.persShadows.reserve(lights.getSpotLights().size());

	for (auto& entity: getSystemEntities()) {
		auto& mat = entity.getComponent<MaterialComponent>();

		if (mat.flags & Opaque) {
			mEntities.push_back(entity);}
	}

	mShadowData.dirShadow.shadowMap = mDirShadowPass->getShadowMap();
	if (const auto light = lights.getDirLight(); light) {
		mDirShadowPass->render(mEntities, light->direction);
		mShadowData.dirShadow.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	}

	if (lights.getPointLights().empty()) {
		// We also have to set the unused depth map, otherwise OpenGL binds it to slot 0
		mShadowData.omnidirShadows.emplace_back(0.0, mOmnidirShadowPass->getShadowMap());
	} else {
		for (auto& light: lights.getPointLights()) {
			mOmnidirShadowPass->render(mEntities, light->position);
			mShadowData.omnidirShadows.emplace_back(SHADOW_OMNIDIRECTIONAL_FAR, mOmnidirShadowPass->getShadowMap());
		}
	}

	if (lights.getSpotLights().empty()) {
		// We also have to set the unused depth map, otherwise OpenGL binds it to slot 0
		mShadowData.persShadows.emplace_back(
			glm::mat4(1.0f),
			mPerspectiveShadowPass->getShadowMap());
	} else {
		for (auto& light: lights.getSpotLights()) {
			mPerspectiveShadowPass->render(mEntities, light->direction, light->position, light->cutOff.y);
			mShadowData.persShadows.emplace_back(
				mPerspectiveShadowPass->getLightSpaceMatrix(),
				mPerspectiveShadowPass->getShadowMap());
		}
	}
}
