#include "renderPipeline.h"
#include "glm/gtc/type_ptr.hpp"
#include "shader.h"
#include "renderFlags.hpp"
#include "buffers/frameBuffer.h"
#include "buffers/uniformBuffer.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../rendering/renderers/forwardRenderer.h"
#include "../rendering/renderers/deferredRenderer.h"
#include "../rendering/renderers/debugRenderer.h"
#include "../rendering/lightSystem.h"
#include "../rendering/postProcess/postProcess.h"
#include "../rendering/skyboxSystem.h"
#include "../rendering/shadowPass/shadowSystem.h"
#include "../rendering/buffers/uniformBuffer.h"
#include "../core/camera.h"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/instance.hpp"
#include "glm/gtx/norm.hpp"


RenderPipeline::RenderPipeline(Registry* registry) {
	RequireComponent<MeshComponent>();
	RequireComponent<ShaderComponent>();
	RequireComponent<TransformComponent>();
	// registry->addSystem<DeferredRenderer>();
	// mDeferredRenderer = &registry->getSystem<DeferredRenderer>();

	registry->addSystem<DebugRenderer>();
	mDebugRenderer = &registry->getSystem<DebugRenderer>();

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();

	registry->addSystem<ShadowSystem>();
	mShadowSystem = &registry->getSystem<ShadowSystem>();

	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mSceneBuffer->withTextureMultisampled(MULTISAMPLED_COUNT)
			.withRenderBufferDepthMultisampled(MULTISAMPLED_COUNT, GL_DEPTH_COMPONENT24)
			.checkStatus();
	mSceneBuffer->unbind();

	mHDRBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mHDRBuffer->withTexture16F()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mHDRBuffer->unbind();

	mIntermediateBuffer = std::make_unique<FrameBuffer>(mSceneBuffer->width(), mSceneBuffer->height());
	mIntermediateBuffer->withTexture()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mIntermediateBuffer->unbind();

	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);

	mForwardRenderer = std::make_unique<ForwardRenderer>();
	mPostProcess = std::make_unique<PostProcess>(mSceneBuffer->width(), mSceneBuffer->height());
}

RenderPipeline::~RenderPipeline() = default;

PostProcess& RenderPipeline::postProcess() const { return *mPostProcess; }

void RenderPipeline::configure(const Camera& camera) const {
	mForwardRenderer->configure(renderQueue.opaqueInstancedEntities,
	                            renderQueue.transparentInstancedEntities);
	//mDeferredRenderer->configure(mLightSystem->getLightUBO());
	mDebugRenderer->configure(*mCameraUBO);
	mShadowSystem->configure();

	// Uniform buffer configuration
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		mCameraUBO->configure(shader->ID(), 0, "CameraBlock");
		mLightSystem->getLightUBO().configure(shader->ID(), 1, "LightBlock");
	}

	const glm::mat4 projectionMat = glm::perspective(
		glm::radians(camera.zoom()),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		ZNEAR, ZFAR);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void RenderPipeline::batchEntities(const Camera& camera) {
	for (const auto& entity: getSystemEntities()) {
		batchEntities(entity, camera);
	}
}

void RenderPipeline::render(const Camera& camera) {
	mLightSystem->update();
	mShadowSystem->shadowPass(*mLightSystem);
	updateBuffers(camera);

	beginSceneRender();
	mSkyboxSystem->render(camera);
	//mDeferredRenderer->geometryPass();
	//mDeferredRenderer->lightingPass(mShadowSystem->getShadowMaps());
	mForwardRenderer->opaquePass(renderQueue.opaqueBatches, mShadowSystem->getShadowMaps());
	mForwardRenderer->instancedPass(renderQueue.transparentInstancedEntities);
	mDebugRenderer->render();
	mForwardRenderer->transparentPass(renderQueue.transparentEntities);
	mForwardRenderer->transparentInstancedPass(renderQueue.transparentInstancedEntities, camera);
	endSceneRender();
#ifdef HDR
	mPostProcess->render(mHDRBuffer->texture());
#else
	mPostProcess->render(mIntermediateBuffer->texture());
#endif
}

void RenderPipeline::updateBuffers(const Camera& camera) const {
	auto view = camera.viewMatrix();
	auto viewPos = glm::vec4(camera.position(), 1.0);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(view), sizeof(glm::mat4), 0);
	mCameraUBO->setData(glm::value_ptr(viewPos), sizeof(glm::vec4), 2 * sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void RenderPipeline::beginSceneRender() const {
#ifdef HDR
	mHDRBuffer->bind();
#else
	mSceneBuffer->bind();
#endif
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderPipeline::endSceneRender() const {
#ifdef HDR
	mHDRBuffer->unbind();
#else
	mSceneBuffer->unbind();
	mSceneBuffer->bindForRead();
	mIntermediateBuffer->bindForDraw();
	glBlitFramebuffer(0, 0, mSceneBuffer->width(), mSceneBuffer->height(), 0, 0, mIntermediateBuffer->width(),
	                  mIntermediateBuffer->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPipeline::batchEntities(const Entity& entity, const Camera& camera) {
	const auto& mat = entity.getComponent<MaterialComponent>();

	if (mat.flags & Instanced) {
		if (mat.flags & Transparent) {
			renderQueue.transparentInstancedEntities.push_back(entity);
		} else
			renderQueue.opaqueInstancedEntities.push_back(entity);
	} else {
		if (mat.flags & Transparent) {
			const auto& tc = entity.getComponent<TransformComponent>();
			float distance = glm::length2(camera.position() - tc.position);
			renderQueue.transparentEntities.emplace_back(distance, entity);
		} else {
			auto& shader = *entity.getComponent<ShaderComponent>().shader;
			renderQueue.opaqueBatches[&shader].push_back(entity);
		}
	}
}
