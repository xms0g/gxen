#include "perspectiveShadowPass.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../lightSystem.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../../config/config.hpp"

PerspectiveShadowPass::PerspectiveShadowPass(int mapWidth, int mapHeight) {
	mDepthMap = std::make_unique<FrameBuffer>(mapWidth, mapHeight);
	mDepthMap->withTextureDepth()
			.checkStatus();
	mDepthMap->unbind();

	mDepthShader = std::make_unique<Shader>("depth/depth.vert", "depth/depth.frag");
}

PerspectiveShadowPass::~PerspectiveShadowPass() = default;

uint32_t PerspectiveShadowPass::getShadowMap() const {
	return mDepthMap->texture();
}

glm::mat4 PerspectiveShadowPass::getLightSpaceMatrix() const {
	return mLightSpaceMatrix;
}

void PerspectiveShadowPass::render(const std::vector<Entity>& entities, const glm::vec3& dir, const glm::vec3& pos,
                                   const float fovy) {
	const glm::mat4 lightProjection = glm::perspective(
		fovy,
		static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT),
		SHADOW_PERSPECTIVE_NEAR,
		SHADOW_PERSPECTIVE_FAR);
	const glm::mat4 lightView = glm::lookAt(pos, pos + dir, glm::vec3(0.0, 1.0, 0.0));
	mLightSpaceMatrix = lightProjection * lightView;

	mDepthShader->activate();
	mDepthShader->setMat4("lightSpaceMatrix", mLightSpaceMatrix);

	// render scene from light's point of view
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	mDepthMap->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	for (const auto& entity: entities) {
		opaquePass(entity, *mDepthShader);
	}
	mDepthMap->unbind();
	glCullFace(GL_BACK);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
