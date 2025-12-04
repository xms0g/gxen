#include "shadowPass.h"
#include "glad/glad.h"
#include "../../renderContext/renderContext.hpp"
#include "../../buffers/uniformBuffer.h"
#include "../../buffers/frameBuffer.h"
#include "../../../ECS/components/directionalLight.hpp"
#include "../../../ECS/components/pointLight.hpp"
#include "../../../ECS/components/spotLight.hpp"

ShadowPass::~ShadowPass() = default;

void ShadowPass::configure(const RenderContext& context) {
	mDirShadowPass = std::make_unique<DirectionalShadowPass>(context);
	mOmnidirShadowPass = std::make_unique<OmnidirectionalShadowPass>(context);
	mPerspectiveShadowPass = std::make_unique<PerspectiveShadowPass>(context);

	mShadowUBO = std::make_unique<UniformBuffer>(sizeof(ShadowData), 2);

	mShadowMaps = {
		mDirShadowPass->getDepthTexture(),
		mOmnidirShadowPass->getDepthTexture(),
		mPerspectiveShadowPass->getDepthTexture()
	};
}

void ShadowPass::execute(const RenderContext& context) {
	directionalShadowPass(context);
	omnidirectionalShadowPass(context);
	perspectiveShadowPass(context);

	mShadowUBO->bind();
	mShadowUBO->setData(&mShadowData, sizeof(ShadowData), 0);
	mShadowUBO->unbind();
}

const std::array<uint32_t, 3>& ShadowPass::getShadowMaps() const { return mShadowMaps; }

const UniformBuffer* ShadowPass::getShadowUBO() const { return mShadowUBO.get(); }

void ShadowPass::directionalShadowPass(const RenderContext& context) {
	for (const auto& light: *context.light.dirLights) {
		mDirShadowPass->render(context, light->direction);
		mShadowData.lightSpaceMatrix = mDirShadowPass->getLightSpaceMatrix();
	}
}

void ShadowPass::omnidirectionalShadowPass(const RenderContext& context) {
	mOmnidirShadowPass->getDepthMap().bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, context.shadowMap.width, context.shadowMap.height);

	const auto& lights = *context.light.pointLights;
	for (int i = 0; i < lights.size(); i++) {
		const auto& light = lights[i];
		if (!light->castShadow) continue;

		mOmnidirShadowPass->render(context, light->position, i);
		mShadowData.omniFarPlanes[i] = context.shadowMap.omnidirectional.farPlane;
	}

	mOmnidirShadowPass->getDepthMap().unbind();
	glViewport(0, 0, static_cast<int32_t>(context.screen.width), static_cast<int32_t>(context.screen.height));
}

void ShadowPass::perspectiveShadowPass(const RenderContext& context) {
	const auto& lights = *context.light.spotLights;
	for (int i = 0; i < lights.size(); i++) {
		const auto& light = lights[i];
		if (!light->castShadow) continue;

		mPerspectiveShadowPass->render(context, light->direction, light->position, light->cutOff.y, i);
		mShadowData.persLightSpaceMatrix[i] = mPerspectiveShadowPass->getLightSpaceMatrix(i);
	}
}
