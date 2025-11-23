#include "directionalShadowPass.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../shader.h"
#include "../renderItem.hpp"
#include "../buffers/frameBuffer.h"
#include "../renderers/renderCommon.h"
#include "../../config/config.hpp"
#include "../../ECS/entity.hpp"

DirectionalShadowPass::DirectionalShadowPass(int mapWidth, int mapHeight) {
	mDepthMap = std::make_unique<FrameBuffer>(mapWidth, mapHeight);
	mDepthMap->withTextureDepth()
			.checkStatus();
	mDepthMap->unbind();

	mDepthShader = std::make_unique<Shader>("depth/depth.vert", "depth/depth.frag");
}

DirectionalShadowPass::~DirectionalShadowPass() = default;

uint32_t DirectionalShadowPass::getDepthTexture() const {
	return mDepthMap->texture();
}

glm::mat4 DirectionalShadowPass::getLightSpaceMatrix() const {
	return mLightSpaceMatrix;
}

void DirectionalShadowPass::render(const std::vector<RenderItem>& shadowCasters, const glm::vec4& direction) {
	const glm::vec3 lightPos = -glm::vec3(direction) * 5.0f;
	const glm::mat4 lightProjection = glm::ortho(
		SHADOW_DIRECTIONAL_LEFT,
		SHADOW_DIRECTIONAL_RIGHT,
		SHADOW_DIRECTIONAL_BOTTOM,
		SHADOW_DIRECTIONAL_TOP,
		SHADOW_DIRECTIONAL_NEAR,
		SHADOW_DIRECTIONAL_FAR);
	const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	mLightSpaceMatrix = lightProjection * lightView;

	mDepthShader->activate();
	mDepthShader->setMat4("lightSpaceMatrix", mLightSpaceMatrix);

	// render scene from light's point of view
	mDepthMap->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (const auto& item: shadowCasters) {
		RenderCommon::setupTransform(*item.entity, *mDepthShader);
		RenderCommon::drawMesh(item, *mDepthShader);
	}
	mDepthMap->unbind();
	glCullFace(GL_BACK);
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
}
