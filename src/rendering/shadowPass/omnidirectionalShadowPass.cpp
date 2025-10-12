#include "omnidirectionalShadowPass.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../lightSystem.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../../config/config.hpp"
#include "../../ECS/components/pointLight.hpp"

OmnidirectionalShadowPass::OmnidirectionalShadowPass(int mapWidth, int mapHeight) {
	mDepthMap = std::make_unique<FrameBuffer>(mapWidth, mapHeight);
	mDepthMap->withTextureCubeMapDepth()
			.checkStatus();
	mDepthMap->unbind();

	mDepthShader = std::make_unique<Shader>("depth/depthCubemap.vert", "depth/depthCubemap.frag",
	                                        "depth/depthCubemap.geom");
}

OmnidirectionalShadowPass::~OmnidirectionalShadowPass() = default;

uint32_t OmnidirectionalShadowPass::getShadowMap() const {
	return mDepthMap->texture();
}

void OmnidirectionalShadowPass::render(const std::vector<Entity>& entities, const glm::vec3& pos) const {
	const glm::mat4 shadowProj = glm::perspective(
		glm::radians(SHADOW_OMNIDIRECTIONAL_FOVY),
		(float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
		SHADOW_OMNIDIRECTIONAL_NEAR,
		SHADOW_OMNIDIRECTIONAL_FAR);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, -1.0f, 0.0f)));

	mDepthShader->activate();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	mDepthMap->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	for (unsigned int i = 0; i < 6; ++i)
		mDepthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

	mDepthShader->setFloat("omniFarPlane", SHADOW_OMNIDIRECTIONAL_FAR);
	mDepthShader->setVec3("lightPos", pos);

	for (const auto& entity: entities) {
		opaquePass(entity, *mDepthShader);
	}
	mDepthMap->unbind();
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
