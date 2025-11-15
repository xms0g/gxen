#include "omnidirectionalShadowPass.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../lightSystem.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../renderers/renderCommon.h"
#include "../../config/config.hpp"

OmnidirectionalShadowPass::OmnidirectionalShadowPass(int mapWidth, int mapHeight) {
	mDepthMap = std::make_unique<FrameBuffer>(mapWidth, mapHeight);
	mDepthMap->withTextureCubemapArrayDepth(MAX_POINT_LIGHTS)
			.checkStatus();
	mDepthMap->unbind();

	mDepthShader = std::make_unique<Shader>("depth/depthCubemap.vert", "depth/depthCubemap.frag",
	                                        "depth/depthCubemap.geom");
}

OmnidirectionalShadowPass::~OmnidirectionalShadowPass() = default;

uint32_t OmnidirectionalShadowPass::getDepthTexture() const {
	return mDepthMap->texture();
}

FrameBuffer& OmnidirectionalShadowPass::getDepthMap() const {
	return *mDepthMap;
}

void OmnidirectionalShadowPass::render(const std::vector<Entity>& entities, const glm::vec4& position,
                                       const int layer) const {
	const glm::mat4 shadowProj = glm::perspective(
		glm::radians(SHADOW_OMNIDIRECTIONAL_FOVY),
		static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT),
		SHADOW_OMNIDIRECTIONAL_NEAR,
		SHADOW_OMNIDIRECTIONAL_FAR);

	const auto pos = glm::vec3(position);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	mDepthShader->activate();
	for (unsigned int i = 0; i < 6; ++i)
		mDepthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

	mDepthShader->setFloat("omniFarPlane", SHADOW_OMNIDIRECTIONAL_FAR);
	mDepthShader->setVec3("lightPos", position);
	mDepthShader->setInt("cubeIndex", layer);

	for (const auto& entity: entities) {
		RenderCommon::setupTransform(entity, *mDepthShader);
		RenderCommon::drawMeshes(entity, *mDepthShader);
	}
}
