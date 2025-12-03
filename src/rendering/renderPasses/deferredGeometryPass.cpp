#include "deferredGeometryPass.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../renderers/renderCommon.h"
#include "../buffers/frameBuffer.h"
#include "../buffers/uniformBuffer.h"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/entity.hpp"

DeferredGeometryPass::~DeferredGeometryPass() = default;

void DeferredGeometryPass::configure(const RenderContext& context) {
	mGBuffer = std::make_unique<FrameBuffer>(context.screen.width, context.screen.height);
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

	mShader = std::make_unique<Shader>("deferred/gbuffer.vert", "deferred/gbuffer.frag");
	context.cameraUBO->configure(mShader->ID(), 0, "CameraBlock");
}

void DeferredGeometryPass::execute(const RenderContext& context) {
	mGBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mShader->activate();
	for (const auto& [entity, matBatches]: context.renderQueue->deferredGroups) {
		if (!entity->getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		for (const auto& [material, shader, meshes]: matBatches) {
			RenderCommon::setupTransform(*entity, *mShader);
			RenderCommon::setupMaterial(*entity, *mShader);

			RenderCommon::bindTextures(material->textures, *mShader);
			RenderCommon::drawMeshes(*meshes);
			RenderCommon::unbindTextures(material->textures);
		}
	}
	mGBuffer->unbind();
}
