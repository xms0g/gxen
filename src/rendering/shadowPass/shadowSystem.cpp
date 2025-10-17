#include "shadowSystem.h"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../shader.h"
#include "../lightSystem.h"
#include "../renderFlags.hpp"
#include "../buffers/uniformBuffer.h"
#include "../../config/config.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/shader.hpp"

ShadowSystem::ShadowSystem() {
	RequireComponent<MaterialComponent>();

	mDirShadowPass = std::make_unique<DirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(SHADOW_WIDTH, SHADOW_HEIGHT);

	mShadowUBO = std::make_unique<UniformBuffer>(sizeof(ShadowData), 2);

	mShadowMaps = {{
			mDirShadowPass->getShadowMap(),
			mOmnidirShadowPass->getShadowMap(),
			mPerspectiveShadowPass->getShadowMap()
	}};
}

ShadowSystem::~ShadowSystem() = default;

void ShadowSystem::configure() const {
	for (auto& entity: getSystemEntities()) {
		const auto& mat = entity.getComponent<MaterialComponent>();

		if (mat.flags & Opaque) {
			const auto& shader = entity.getComponent<ShaderComponent>().shader;
			mShadowUBO->configure(shader->ID(), 2, "ShadowBlock");
		}
	}
}

void ShadowSystem::shadowPass(const LightSystem& lights) {
	mEntities.clear();

	for (auto& entity: getSystemEntities()) {
		const auto& mat = entity.getComponent<MaterialComponent>();

		if (mat.flags & Opaque) {
			mEntities.push_back(entity);
		}
	}

	for (auto& light: lights.getDirLights()) {
		mDirShadowPass->render(mEntities, light->direction);
		mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	}

	for (int i = 0; i < lights.getPointLights().size(); i++) {
		const auto& light = lights.getPointLights()[i];
		mOmnidirShadowPass->render(mEntities, light->position);
		mShadowData.omniFarPlanes[i] = SHADOW_OMNIDIRECTIONAL_FAR;
	}

	for (auto& light: lights.getSpotLights()) {
		mPerspectiveShadowPass->render(mEntities, light->direction, light->position, light->cutOff.y);
		mShadowData.persLightSpaceMatrix = mPerspectiveShadowPass->getLightSpaceMatrix();
	}

	mShadowUBO->bind();
	mShadowUBO->setData(&mShadowData, sizeof(ShadowData), 0);
	mShadowUBO->unbind();
}
