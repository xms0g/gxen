#include "renderPipeline.h"
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "shader.h"
#include "lightSystem.h"
#include "renderFlags.hpp"
#include "skyboxSystem.h"
#include "buffers/frameBuffer.h"
#include "buffers/uniformBuffer.h"
#include "renderers/forwardRenderer.h"
#include "renderers/deferredRenderer.h"
#include "renderers/debugRenderer.h"
#include "postProcess/postProcess.h"
#include "shadowPass/shadowManager.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../core/camera.h"
#include "../ECS/components/debug.hpp"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/bv.hpp"
#include "../ECS/components/instance.hpp"
#include "../math/frustum.hpp"
#include "../math/boundingVolume.h"

RenderPipeline::RenderPipeline(Registry* registry) {
	RequireComponent<MeshComponent>();
	RequireComponent<ShaderComponent>();
	RequireComponent<TransformComponent>();
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		throw std::runtime_error(std::string("ERROR::RENDERER::FAILED_TO_INIT_GLAD"));
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();
#ifdef HDR
	mHDRBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mHDRBuffer->withTexture16F()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mHDRBuffer->unbind();
#else
	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
# ifdef ANTIALIASING
	glEnable(GL_MULTISAMPLE);
	mIntermediateBuffer = std::make_unique<FrameBuffer>(mSceneBuffer->width(), mSceneBuffer->height());
	mIntermediateBuffer->withTexture()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mIntermediateBuffer->unbind();

	mSceneBuffer->withTextureMultisampled(MULTISAMPLED_COUNT)
			.withRenderBufferDepthMultisampled(MULTISAMPLED_COUNT, GL_DEPTH_COMPONENT24)
#else
	mSceneBuffer->withTexture()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
# endif
			.checkStatus();
	mSceneBuffer->unbind();
#endif

#ifdef DEFERRED
	mGBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mGBuffer->withTexture16F()
			.withTexture16F()
			.withTexture()
			.configureAttachments()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mDeferredLigthingShader = std::make_unique<Shader>("models/quad.vert", "deferred/lighting.frag");
	mGShader = std::make_unique<Shader>("deferred/gbuffer.vert", "deferred/gbuffer.frag");
	mDeferredRenderer = std::make_unique<DeferredRenderer>(*mDeferredLigthingShader);
#endif
	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);

	mForwardRenderer = std::make_unique<ForwardRenderer>();
	mShadowManager = std::make_unique<ShadowManager>();
	mDebugRenderer = std::make_unique<DebugRenderer>();
	mPostProcess = std::make_unique<PostProcess>(SCR_WIDTH, SCR_HEIGHT);
}

RenderPipeline::~RenderPipeline() = default;

PostProcess& RenderPipeline::postProcess() const { return *mPostProcess; }

void RenderPipeline::configure(const Camera& camera) const {
	mForwardRenderer->configure(renderQueues.opaqueInstancedEntities, renderQueues.transparentInstancedEntities);
	mDebugRenderer->configure(*mCameraUBO);

	// Uniform buffer configuration
#ifdef DEFERRED
	mCameraUBO->configure(mGShader->ID(), 0, "CameraBlock");
	mCameraUBO->configure(mDeferredLigthingShader->ID(), 0, "CameraBlock");
	mLightSystem->getLightUBO().configure(mDeferredLigthingShader->ID(), 1, "LightBlock");
	mShadowManager->getShadowUBO().configure(mDeferredLigthingShader->ID(), 2, "ShadowBlock");
#endif
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		mCameraUBO->configure(shader->ID(), 0, "CameraBlock");
		mLightSystem->getLightUBO().configure(shader->ID(), 1, "LightBlock");
		mShadowManager->getShadowUBO().configure(shader->ID(), 2, "ShadowBlock");
	}

	const glm::mat4 projectionMat = glm::perspective(
		glm::radians(camera.zoom()),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		ZNEAR, ZFAR);

	mCameraUBO->bind();
	mCameraUBO->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mCameraUBO->unbind();
}

void RenderPipeline::batchEntities() {
	for (const auto& entity: getSystemEntities()) {
		batchEntities(entity);
	}
}

void RenderPipeline::render(const Camera& camera) {
	updateBuffers(camera);
	frustumCullingPass(camera);
	sortEntities(camera);

	mLightSystem->update();
	mShadowManager->shadowPass(renderQueues.opaqueBatches, *mLightSystem);

	beginSceneRender();
#ifdef DEFERRED
	mDeferredRenderer->geometryPass(renderQueues.opaqueBatches, *mGBuffer, *mGShader);
# ifdef HDR
	mDeferredRenderer->lightingPass(mShadowManager->getShadowMaps(), *mGBuffer, *mHDRBuffer,
	                                *mDeferredLigthingShader);
# else
	mDeferredRenderer->lightingPass(mShadowManager->getShadowMaps(), *mGBuffer, *mSceneBuffer,
									*mDeferredLigthingShader);
# endif
#else
	mForwardRenderer->opaquePass(renderQueues.opaqueBatches, mShadowManager->getShadowMaps());
#endif
	mDebugRenderer->render(renderQueues.debugEntities);
	mForwardRenderer->opaqueInstancedPass(renderQueues.opaqueInstancedEntities, mShadowManager->getShadowMaps());

	mSkyboxSystem->render(camera);

	mForwardRenderer->transparentPass(renderQueues.transparentEntities);
	mForwardRenderer->transparentInstancedPass(renderQueues.transparentInstancedEntities);
	endSceneRender();
#ifdef HDR
	mPostProcess->render(mHDRBuffer->texture());
#else
# ifdef ANTIALIASING
	mPostProcess->render(mIntermediateBuffer->texture());
# else
	mPostProcess->render(mSceneBuffer->texture());
# endif
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

void RenderPipeline::frustumCullingPass(const Camera& camera) const {
	const math::Frustum& frustum = camera.frustum();

	auto cullEntity = [&](const Entity& entity) {
		auto& bvc = entity.getComponent<BoundingVolumeComponent>();
		const auto& tc = entity.getComponent<TransformComponent>();
		bvc.isVisible = bvc.bv->isOnFrustum(frustum, tc.position, tc.rotation, tc.scale);
	};

	for (const auto& [shader, entities]: renderQueues.opaqueBatches) {
		for (const auto& entity: entities) {
			cullEntity(entity);
		}
	}

	for (const auto& entity: renderQueues.opaqueInstancedEntities) {
		cullEntity(entity);
	}

	for (const auto& entity: renderQueues.transparentEntities) {
		cullEntity(entity);
	}

	for (const auto& entity: renderQueues.transparentInstancedEntities) {
		cullEntity(entity);
	}

	for (const auto& entity: renderQueues.debugEntities) {
		cullEntity(entity);
	}
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
# ifdef ANTIALIASING
	mSceneBuffer->bindForRead();
	mIntermediateBuffer->bindForDraw();
	glBlitFramebuffer(0, 0, mSceneBuffer->width(), mSceneBuffer->height(),
	                  0, 0, mIntermediateBuffer->width(), mIntermediateBuffer->height(),
	                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
# endif
#endif
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPipeline::batchEntities(const Entity& entity) {
	if (entity.hasComponent<DebugComponent>()) {
		renderQueues.debugEntities.push_back(entity);
	}

	const auto& mat = entity.getComponent<MaterialComponent>();

	if (mat.flags & Instanced) {
		if (mat.flags & Transparent) {
			renderQueues.transparentInstancedEntities.push_back(entity);
		} else {
			renderQueues.opaqueInstancedEntities.push_back(entity);
		}
	} else {
		if (mat.flags & Transparent) {
			renderQueues.transparentEntities.emplace_back(entity);
		} else {
			auto& shader = *entity.getComponent<ShaderComponent>().shader;
			renderQueues.opaqueBatches[&shader].push_back(entity);
		}
	}
}

void RenderPipeline::sortEntities(const Camera& camera) {
	const glm::vec3 camPos = camera.position();

	// Sort opaque objects front to back
	for (auto& [shader, entities]: renderQueues.opaqueBatches) {
		std::sort(
			entities.begin(),
			entities.end(),
			[&camPos](const Entity& a, const Entity& b) {
				const float da = glm::length2(camPos - a.getComponent<TransformComponent>().position);
				const float db = glm::length2(camPos - b.getComponent<TransformComponent>().position);
				return da < db;
			}
		);
	}

	std::sort(
		renderQueues.transparentEntities.begin(),
		renderQueues.transparentEntities.end(),
		[&camPos](const Entity& a, const Entity& b) {
			const float da = glm::length2(camPos - a.getComponent<TransformComponent>().position);
			const float db = glm::length2(camPos - b.getComponent<TransformComponent>().position);
			return da > db;
		}
	);

	for (auto& entity: renderQueues.transparentInstancedEntities) {
		const auto& positions = entity.getComponent<InstanceComponent>().positions;

		std::sort(
			positions->begin(),
			positions->end(),
			[&camPos](const glm::vec3& a, const glm::vec3& b) {
				const float da = glm::length2(camPos - a);
				const float db = glm::length2(camPos - b);
				return da > db; // back to front
			}
		);
	}
}
