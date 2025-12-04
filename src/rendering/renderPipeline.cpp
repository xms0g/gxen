#include "renderPipeline.h"
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "shader.h"
#include "lightSystem.h"
#include "skyboxSystem.h"
#include "buffers/frameBuffer.h"
#include "buffers/uniformBuffer.h"
#include "renderContext/renderContext.hpp"
#include "renderContext/renderFlags.hpp"
#include "renderContext/renderGroup.hpp"
#include "renderContext/instanceGroup.hpp"
#include "renderPasses/shadowPass/shadowPass.h"
#include "renderPasses/IRenderPass.hpp"
#include "renderPasses/deferredGeometryPass.h"
#include "renderPasses/deferredLightingPass.h"
#include "renderPasses/debugPass.h"
#include "renderPasses/forwardOpaquePass.h"
#include "renderPasses/blendInstancedPass.h"
#include "renderPasses/blendPass.h"
#include "renderPasses/opaqueInstancedPass.h"
#include "renderPasses/beginScenePass.h"
#include "renderPasses/frustumCullingPass.h"
#include "postProcess/postProcess.h"
#include "material/material.hpp"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../core/camera.h"
#include "../ECS/components/debug.hpp"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/instance.hpp"

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

	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
#ifdef MSAA
	glEnable(GL_MULTISAMPLE);
	mIntermediateBuffer = std::make_unique<FrameBuffer>(mSceneBuffer->width(), mSceneBuffer->height());
# ifdef HDR
	mIntermediateBuffer->withTexture16F()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mIntermediateBuffer->unbind();

	mSceneBuffer->bind();
	mSceneBuffer->withTexture16FMultisampled(MULTISAMPLED_COUNT)
# else
	mIntermediateBuffer->withTexture()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mIntermediateBuffer->unbind();

	mSceneBuffer->bind();
	mSceneBuffer->withTextureMultisampled(MULTISAMPLED_COUNT)
# endif
			.withRenderBufferDepthMultisampled(MULTISAMPLED_COUNT, GL_DEPTH_COMPONENT24)
#else
# ifdef HDR
	mSceneBuffer->withTexture16F()
# else
	mSceneBuffer->withTexture()
# endif
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
#endif
			.checkStatus();
	mSceneBuffer->unbind();

	mShadowPass = std::make_shared<ShadowPass>();
	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);
	mPostProcess = std::make_unique<PostProcess>(SCR_WIDTH, SCR_HEIGHT);
}

RenderPipeline::~RenderPipeline() = default;

PostProcess& RenderPipeline::postProcess() const { return *mPostProcess; }

void RenderPipeline::configure(const Camera& camera) {
	mContext = std::make_unique<RenderContext>(&mRenderQueue, mSceneBuffer.get());
	mContext->light.ubo = &mLightSystem->getLightUBO();
	mContext->light.dirLights = &mLightSystem->getDirLights();
	mContext->light.pointLights = &mLightSystem->getPointLights();
	mContext->light.spotLights = &mLightSystem->getSpotLights();
	mContext->camera.self = &camera;
	mContext->camera.ubo = mCameraUBO.get();
	mContext->screen.width = SCR_WIDTH;
	mContext->screen.height = SCR_HEIGHT;
	mContext->shadowMap.textureSlot = SHADOWMAP_TEXTURE_SLOT;
	mContext->shadowMap.width = SHADOWMAP_WIDTH;
	mContext->shadowMap.height = SHADOWMAP_HEIGHT;
	mContext->shadowMap.directional.nearPlane = SHADOW_DIRECTIONAL_NEAR;
	mContext->shadowMap.directional.farPlane = SHADOW_DIRECTIONAL_FAR;
	mContext->shadowMap.directional.left = SHADOW_DIRECTIONAL_LEFT;
	mContext->shadowMap.directional.right = SHADOW_DIRECTIONAL_RIGHT;
	mContext->shadowMap.directional.bottom = SHADOW_DIRECTIONAL_BOTTOM;
	mContext->shadowMap.directional.top = SHADOW_DIRECTIONAL_TOP;
	mContext->shadowMap.omnidirectional.maxLights = MAX_POINT_LIGHTS;
	mContext->shadowMap.omnidirectional.nearPlane = SHADOW_OMNIDIRECTIONAL_NEAR;
	mContext->shadowMap.omnidirectional.farPlane = SHADOW_OMNIDIRECTIONAL_FAR;
	mContext->shadowMap.omnidirectional.fovy = SHADOW_OMNIDIRECTIONAL_FOVY;
	mContext->shadowMap.perspective.maxLights = MAX_SPOT_LIGHTS;
	mContext->shadowMap.perspective.nearPlane = SHADOW_PERSPECTIVE_NEAR;
	mContext->shadowMap.perspective.farPlane = SHADOW_PERSPECTIVE_FAR;
	mContext->shadowMap.ubo = mShadowPass->getShadowUBO();
	mContext->shadowMap.textures = &mShadowPass->getShadowMaps();

	// Create render passes
	mRenderPasses.push_back(std::make_shared<FrustumCullingPass>());
	mRenderPasses.push_back(mShadowPass);
	mRenderPasses.push_back(std::make_shared<BeginScenePass>());

	if (!mRenderQueue.deferredGroups.empty()) {
		mDeferredGeometryPass = std::make_shared<DeferredGeometryPass>();
		mDeferredLightingPass = std::make_shared<DeferredLightingPass>();

		mRenderPasses.push_back(mDeferredGeometryPass);
		mRenderPasses.push_back(mDeferredLightingPass);
	}

	if (!mRenderQueue.forwardOpaqueGroups.empty()) {
		mRenderPasses.push_back(std::make_shared<ForwardOpaquePass>());
	}

	if (!mRenderQueue.debugGroups.empty()) {
		mRenderPasses.push_back(std::make_shared<DebugPass>());
	}

	if (!mRenderQueue.opaqueInstancedGroups.empty()) {
		mRenderPasses.push_back(std::make_shared<OpaqueInstancedPass>());
	}

	if (!mRenderQueue.blendGroups.empty()) {
		mRenderPasses.push_back(std::make_shared<BlendPass>());
	}

	if (!mRenderQueue.blendInstancedGroups.empty()) {
		mRenderPasses.push_back(std::make_shared<BlendInstancedPass>());
	}

	// Configure render passes
	for (const auto& pass: mRenderPasses) {
		pass->configure(*mContext);
	}

	if (!mRenderQueue.deferredGroups.empty()) {
		mDeferredLightingPass->configureInput(mDeferredGeometryPass->getGBuffer());
	}

	// Configure entities
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		mContext->camera.ubo->configure(shader->ID(), 0, "CameraBlock");
		mContext->light.ubo->configure(shader->ID(), 1, "LightBlock");
		mContext->shadowMap.ubo->configure(shader->ID(), 2, "ShadowBlock");
	}

	const glm::mat4 projectionMat = glm::perspective(
		glm::radians(camera.zoom()),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		ZNEAR, ZFAR);

	mContext->camera.ubo->bind();
	mContext->camera.ubo->setData(glm::value_ptr(projectionMat), sizeof(glm::mat4), sizeof(glm::mat4));
	mContext->camera.ubo->unbind();
}

void RenderPipeline::batchEntities() {
	for (const auto& entity: getSystemEntities()) {
		batchEntity(entity);
	}
}

void RenderPipeline::render() {
	updateBuffers();
	sortEntities();

	mLightSystem->update();

	for (const auto& pass: mRenderPasses) {
		pass->execute(*mContext);
	}

	mSkyboxSystem->render(*mContext->camera.self);
	mSceneBuffer->unbind();
#ifdef MSAA
	mSceneBuffer->bindForRead();
	mIntermediateBuffer->bindForDraw();
	glBlitFramebuffer(0, 0, mSceneBuffer->width(), mSceneBuffer->height(),
					  0, 0, mIntermediateBuffer->width(), mIntermediateBuffer->height(),
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	mPostProcess->render(mIntermediateBuffer->texture());
#else
	mPostProcess->render(mContext->sceneBuffer->texture());
#endif
	glViewport(0, 0, static_cast<int32_t>(SCR_WIDTH), static_cast<int32_t>(SCR_HEIGHT));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPipeline::updateBuffers() const {
	auto view = mContext->camera.self->viewMatrix();
	auto viewPos = glm::vec4(mContext->camera.self->position(), 1.0);

	mContext->camera.ubo->bind();
	mContext->camera.ubo->setData(glm::value_ptr(view), sizeof(glm::mat4), 0);
	mContext->camera.ubo->setData(glm::value_ptr(viewPos), sizeof(glm::vec4), 2 * sizeof(glm::mat4));
	mContext->camera.ubo->unbind();
}

void RenderPipeline::batchEntity(const Entity& entity) {
	const auto& matc = entity.getComponent<MaterialComponent>();
	const auto& shader = *entity.getComponent<ShaderComponent>().shader;
	const auto& materials = entity.getComponent<MaterialComponent>().materials;

	for (auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
		const auto& material = materials->at(matID);
		std::vector<MaterialBatch> matBatch;
		matBatch.emplace_back(&material, &shader, &meshes);

		if (matc.flag & Instanced) {
			const auto& ic = entity.getComponent<InstanceComponent>();
			InstanceGroup instance{&entity, ic.transforms, matBatch};

			if (material.flag & Blend) {
				mRenderQueue.blendInstancedGroups.push_back(instance);
			} else {
				mRenderQueue.opaqueInstancedGroups.push_back(instance);
			}
			continue;
		}

		RenderGroup group{&entity, matBatch};

		if (entity.hasComponent<DebugComponent>()) {
			mRenderQueue.debugGroups.push_back(group);
		}

		if (material.flag & CastShadow) {
			mRenderQueue.shadowCasterGroups.push_back(group);
		}

		if (material.flag & Opaque) {
			if (matc.flag & Forward) {
				mRenderQueue.forwardOpaqueGroups.push_back(group);
			} else {
				mRenderQueue.deferredGroups.push_back(group);
			}
		} else if (material.flag & Cutout) {
			mRenderQueue.forwardOpaqueGroups.push_back(group);
		} else if (material.flag & Blend) {
			mRenderQueue.blendGroups.push_back(group);
		}
	}
}

void RenderPipeline::sortEntities() {
	const glm::vec3& camPos = mContext->camera.self->position();

	auto sortBatches = [&](auto& batch, bool transparent) {
		std::sort(
			batch.begin(),
			batch.end(),
			[&camPos, &transparent](const RenderGroup& a, const RenderGroup& b) {
				const float da = glm::length2(camPos - a.entity->getComponent<TransformComponent>().position);
				const float db = glm::length2(camPos - b.entity->getComponent<TransformComponent>().position);

				if (transparent)
					return da > db;
				return da < db;
			});
	};

	// Sort opaque objects front to back
	sortBatches(mRenderQueue.deferredGroups, false);
	sortBatches(mRenderQueue.forwardOpaqueGroups, false);
	sortBatches(mRenderQueue.blendGroups, true);

	// for (auto& [entity, transforms, materials]: renderQueues.blendInstancedGroup) {
	// 	auto transform = *transforms;
	//
	// 	for (int i = 0; i < transform.size(); i += 9) {
	// 		glm::vec3 pos{transform[i], transform[i + 1], transform[i + 2]};
	// 		std::sort(
	// 			positions->begin(),
	// 			positions->end(),
	// 			[&camPos](const glm::vec3& a, const glm::vec3& b) {
	// 				const float da = glm::length2(camPos - a);
	// 				const float db = glm::length2(camPos - b);
	// 				return da > db; // back to front
	// 			}
	// 		);
	// 	}
	// }
}
