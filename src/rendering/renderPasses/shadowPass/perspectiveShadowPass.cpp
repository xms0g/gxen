#include "perspectiveShadowPass.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../../shader.h"
#include "../../renderContext/renderGroup.hpp"
#include "../../renderContext/renderContext.hpp"
#include "../../renderContext/renderQueue.hpp"
#include "../../buffers/frameBuffer.h"
#include "../../renderCommon.h"
#include "../../../ECS/entity.hpp"

PerspectiveShadowPass::PerspectiveShadowPass(const RenderContext& context) {
	mDepthMap = std::make_unique<FrameBuffer>(context.shadowMap.width, context.shadowMap.height);
	mDepthMap->withTextureArrayDepth(context.shadowMap.perspective.maxLights)
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

void PerspectiveShadowPass::render(const RenderContext& context, const glm::vec4& direction,
                                   const glm::vec4& position, const float fovy, const int layer) {
	const glm::mat4 lightProjection = glm::perspective(
		fovy,
		static_cast<float>(context.shadowMap.width) / static_cast<float>(context.shadowMap.height),
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
	glViewport(0, 0, context.shadowMap.width, context.shadowMap.height);
	for (const auto& [entity, matBatches]: context.renderQueue->shadowCasterGroups) {
		for (const auto& [material, shader, meshes]: matBatches) {
			RenderCommon::setupTransform(*entity, *mDepthShader);
			RenderCommon::drawMeshes(*meshes);
		}
	}
	mDepthMap->unbind();
	glCullFace(GL_BACK);
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
}
