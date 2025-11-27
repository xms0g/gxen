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

DeferredRenderer::DeferredRenderer(const Shader& lightingShader) : mQuad(std::make_unique<Models::SingleQuad>()) {
	lightingShader.activate();
	lightingShader.setInt("gPosition", 0);
	lightingShader.setInt("gNormal", 1);
	lightingShader.setInt("gAlbedoSpec", 2);
	lightingShader.setInt("shadowMap", SHADOWMAP_TEXTURE_SLOT);
	lightingShader.setInt("shadowCubemap", SHADOWMAP_TEXTURE_SLOT + 1);
	lightingShader.setInt("persShadowMap", SHADOWMAP_TEXTURE_SLOT + 2);
}

DeferredRenderer::~DeferredRenderer() = default;

void DeferredRenderer::geometryPass(const std::vector<RenderGroup>& groups,
                                    const FrameBuffer& gBuffer, const Shader& gShader) const {
	gBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gShader.activate();
	for (const auto& [entity, matBatches]: groups) {
		if (!entity->getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		for (const auto& [material, shader, meshes]: matBatches) {
			RenderCommon::setupTransform(*entity, gShader);
			RenderCommon::setupMaterial(*entity, gShader);

			RenderCommon::bindTextures(material->textures, gShader);
			RenderCommon::drawMeshes(*meshes);
			RenderCommon::unbindTextures(material->textures);
		}
	}
	gBuffer.unbind();
}

void DeferredRenderer::lightingPass(const std::array<uint32_t, 3>& shadowMaps, const FrameBuffer& gBuffer,
                                    const FrameBuffer& sceneBuffer, const Shader& lightingShader) const {
	// Copy depth buffer of gBuffer to scene buffer for the proper depth testing
	gBuffer.bindForRead();
	sceneBuffer.bindForDraw();
	glBlitFramebuffer(0, 0, gBuffer.width(), gBuffer.height(),
	                  0, 0, sceneBuffer.width(), sceneBuffer.height(),
	                  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	sceneBuffer.bind();
	lightingShader.activate();

	for (int i = 0; i < gBuffer.textures().size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer.textures()[i]);
	}

	RenderCommon::bindShadowMaps(shadowMaps);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mQuad->VAO());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
