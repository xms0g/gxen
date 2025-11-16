#include "perspectiveShadowPass.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../renderers/renderCommon.h"
#include "../../config/config.hpp"
#include "../../ECS/entity.hpp"

PerspectiveShadowPass::PerspectiveShadowPass(int mapWidth, int mapHeight) {
	mDepthMap = std::make_unique<FrameBuffer>(mapWidth, mapHeight);
	mDepthMap->withTextureArrayDepth(MAX_SPOT_LIGHTS)
			.checkStatus();
	mDepthMap->unbind();

	mDepthShader = std::make_unique<Shader>("depth/depth.vert", "depth/depth.frag");
}

PerspectiveShadowPass::~PerspectiveShadowPass() = default;

uint32_t PerspectiveShadowPass::getDepthTexture() const {
	return mDepthMap->texture();
}

glm::mat4 PerspectiveShadowPass::getLightSpaceMatrix(const int layer) const {
	return mLightSpaceMatrix[layer];
}

void PerspectiveShadowPass::render(const std::vector<Entity>& entities, const glm::vec4& direction,
                                   const glm::vec4& position, const float fovy, const int layer) {
	const glm::mat4 lightProjection = glm::perspective(
		fovy,
		static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT),
		SHADOW_PERSPECTIVE_NEAR,
		SHADOW_PERSPECTIVE_FAR);

	const auto dir = glm::vec3(direction);
	const auto pos = glm::vec3(position);
	const glm::mat4 lightView = glm::lookAt(pos, pos + dir, glm::vec3(0.0, 1.0, 0.0));
	mLightSpaceMatrix[layer] = lightProjection * lightView;

	mDepthShader->activate();
	mDepthShader->setMat4("lightSpaceMatrix", mLightSpaceMatrix[layer]);

	// render scene from light's point of view
	mDepthMap->bind();
	mDepthMap->attachLayer(GL_DEPTH_ATTACHMENT, layer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (const auto& entity: entities) {
		RenderCommon::setupTransform(entity, *mDepthShader);
		RenderCommon::drawMeshes(entity, *mDepthShader);
	}
	mDepthMap->unbind();
	glCullFace(GL_BACK);
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
}
