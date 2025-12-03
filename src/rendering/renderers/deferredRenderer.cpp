#include "deferredRenderer.h"
#include "glad/glad.h"
#include "renderCommon.h"
#include "../shader.h"
#include "../renderGroup.hpp"
#include "../buffers/frameBuffer.h"
#include "../models/quad.h"
#include "../../config/config.hpp"
#include "../../ECS/entity.hpp"
#include "../../ECS/components/bv.hpp"

DeferredRenderer::DeferredRenderer() : mQuad(std::make_unique<Models::SingleQuad>()) {
	mGBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mGBuffer->withTexture16F()
			.withTexture16F()
#ifdef HDR
			.withTexture16F()
#else
			.withTexture()
#endif
			.configureAttachments()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();

	mLigthingShader = std::make_unique<Shader>("models/quad.vert", "deferred/lighting.frag");
	mGShader = std::make_unique<Shader>("deferred/gbuffer.vert", "deferred/gbuffer.frag");

	mLigthingShader->activate();
	mLigthingShader->setInt("gPosition", 0);
	mLigthingShader->setInt("gNormal", 1);
	mLigthingShader->setInt("gAlbedoSpec", 2);
	mLigthingShader->setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
	mLigthingShader->setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
	mLigthingShader->setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);
}

DeferredRenderer::~DeferredRenderer() = default;

const Shader& DeferredRenderer::getGShader() const {
	return *mGShader;
}

const Shader& DeferredRenderer::getLightingShader() const {
	return *mLigthingShader;
}

void DeferredRenderer::geometryPass(const std::vector<RenderGroup>& groups) const {
	mGBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mGShader->activate();
	for (const auto& [entity, matBatches]: groups) {
		if (!entity->getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		for (const auto& [material, shader, meshes]: matBatches) {
			RenderCommon::setupTransform(*entity, *mGShader);
			RenderCommon::setupMaterial(*entity, *mGShader);

			RenderCommon::bindTextures(material->textures, *mGShader);
			RenderCommon::drawMeshes(*meshes);
			RenderCommon::unbindTextures(material->textures);
		}
	}
	mGBuffer->unbind();
}

void DeferredRenderer::lightingPass(const std::array<uint32_t, 3>& shadowMaps, const FrameBuffer& sceneBuffer) const {
	// Copy depth buffer of gBuffer to scene buffer for the proper depth testing
	mGBuffer->bindForRead();
	sceneBuffer.bindForDraw();
	glBlitFramebuffer(0, 0, mGBuffer->width(), mGBuffer->height(),
	                  0, 0, sceneBuffer.width(), sceneBuffer.height(),
	                  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	sceneBuffer.bind();
	mLigthingShader->activate();

	for (int i = 0; i < mGBuffer->textures().size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mGBuffer->textures()[i]);
	}

	RenderCommon::bindShadowMaps(shadowMaps);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mQuad->VAO());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
