#include "renderPipeline.h"
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "shader.h"
#include "lightSystem.h"
#include "renderFlags.hpp"
#include "renderItem.hpp"
#include "instanceGroup.hpp"
#include "skyboxSystem.h"
#include "buffers/frameBuffer.h"
#include "buffers/uniformBuffer.h"
#include "renderers/forwardRenderer.h"
#include "renderers/deferredRenderer.h"
#include "renderers/debugRenderer.h"
#include "postProcess/postProcess.h"
#include "shadowPass/shadowManager.h"
#include "mesh/mesh.h"
#include "material/material.hpp"
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

	mGBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mGBuffer->withTexture16F()
			.withTexture16F()
# ifdef HDR
			.withTexture16F()
# else
			.withTexture()
# endif
			.configureAttachments()
			.withRenderBufferDepth(GL_DEPTH_COMPONENT24)
			.checkStatus();
	mDeferredLigthingShader = std::make_unique<Shader>("models/quad.vert", "deferred/lighting.frag");
	mGShader = std::make_unique<Shader>("deferred/gbuffer.vert", "deferred/gbuffer.frag");
	mDeferredRenderer = std::make_unique<DeferredRenderer>(*mDeferredLigthingShader);

	mCameraUBO = std::make_unique<UniformBuffer>(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 0);

	mForwardRenderer = std::make_unique<ForwardRenderer>();
	mShadowManager = std::make_unique<ShadowManager>();
	mDebugRenderer = std::make_unique<DebugRenderer>();
	mPostProcess = std::make_unique<PostProcess>(SCR_WIDTH, SCR_HEIGHT);
}

RenderPipeline::~RenderPipeline() = default;

PostProcess& RenderPipeline::postProcess() const { return *mPostProcess; }

void RenderPipeline::configure(const Camera& camera) const {
	mForwardRenderer->configure(renderQueues.opaqueInstancedGroup, renderQueues.blendInstancedGroup);
	mDebugRenderer->configure(*mCameraUBO);

	// Uniform buffer configuration
	mCameraUBO->configure(mGShader->ID(), 0, "CameraBlock");
	mCameraUBO->configure(mDeferredLigthingShader->ID(), 0, "CameraBlock");
	mLightSystem->getLightUBO().configure(mDeferredLigthingShader->ID(), 1, "LightBlock");
	mShadowManager->getShadowUBO().configure(mDeferredLigthingShader->ID(), 2, "ShadowBlock");

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
	mShadowManager->shadowPass(renderQueues.shadowCasters, *mLightSystem);

	beginSceneRender();
	mDeferredRenderer->geometryPass(renderQueues.deferredItems, *mGBuffer, *mGShader);
# ifdef HDR
	mDeferredRenderer->lightingPass(mShadowManager->getShadowMaps(), *mGBuffer, *mHDRBuffer,
	                                *mDeferredLigthingShader);
# else
	mDeferredRenderer->lightingPass(mShadowManager->getShadowMaps(), *mGBuffer, *mSceneBuffer,
	                                *mDeferredLigthingShader);
# endif
	mForwardRenderer->opaquePass(renderQueues.forwardOpaqueItems, mShadowManager->getShadowMaps());

	mDebugRenderer->render(renderQueues.debugEntities);
	mForwardRenderer->opaqueInstancedPass(renderQueues.opaqueInstancedGroup, mShadowManager->getShadowMaps());

	mSkyboxSystem->render(camera);

	mForwardRenderer->transparentPass(renderQueues.blendItems);
	mForwardRenderer->transparentInstancedPass(renderQueues.blendInstancedGroup);
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

	auto cullItems = [&](const std::unordered_map<const Shader*, std::vector<RenderItem> >& renderItems) {
		for (const auto& [shader, items]: renderItems) {
			for (auto& item: items) {
				auto& bvc = item.entity->getComponent<BoundingVolumeComponent>();
				const auto& tc = item.entity->getComponent<TransformComponent>();
				const auto& aabb = bvc.bv;
				bvc.isVisible = aabb->isOnFrustum(frustum, tc.position, tc.rotation, tc.scale);

				if (!bvc.isVisible) return;
				item.mesh->setVisible(aabb->isMeshInFrustum(frustum, item.mesh->min(), item.mesh->max(),
				                                            tc.position, tc.rotation, tc.scale));
			}
		}
	};

	cullItems(renderQueues.forwardOpaqueItems);
	cullItems(renderQueues.deferredItems);
	cullItems(renderQueues.blendItems);
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
	const auto& matc = entity.getComponent<MaterialComponent>();
	const auto& shader = *entity.getComponent<ShaderComponent>().shader;
	const auto& ic = entity.getComponent<InstanceComponent>();
	const auto& materials = entity.getComponent<MaterialComponent>().materials;

	for (auto& [matID, meshes]: *entity.getComponent<MeshComponent>().meshes) {
		const auto& material = materials->at(matID);

		if (matc.flag == RenderFlags::Instanced) {
			std::vector<const Mesh*> instanceMeshes;

			for (auto& mesh: meshes) {
				instanceMeshes.push_back(&mesh);

				// if (material.flag & CastShadow) {
				// 	RenderItem renderItem{&entity, &mesh, &material};
				// 	renderQueues.shadowCasters.push_back(renderItem);
				// }
			}
			std::vector<MaterialBatch> instanceMaterials;
			instanceMaterials.emplace_back(&material, &shader, instanceMeshes);

			InstanceGroup instance{&entity, ic.transforms, instanceMaterials};

			if (material.flag & Blend) {
				renderQueues.blendInstancedGroup.push_back(instance);
			} else {
				renderQueues.opaqueInstancedGroup.push_back(instance);
			}
			continue;
		}

		for (auto& mesh: meshes) {
			RenderItem renderItem{&entity, &mesh, &material};

			if (entity.hasComponent<DebugComponent>()) {
				renderQueues.debugEntities.push_back(renderItem);
			}

			if (material.flag & CastShadow) {
				renderQueues.shadowCasters.push_back(renderItem);
			}

			if (material.flag & Opaque) {
				if (matc.flag == RenderFlags::Forward) {
					renderQueues.forwardOpaqueItems[&shader].push_back(renderItem);
				} else {
					renderQueues.deferredItems[&shader].push_back(renderItem);
				}
			} else if (material.flag & Cutout) {
				renderQueues.forwardOpaqueItems[&shader].push_back(renderItem);
			} else if (material.flag & Blend) {
				renderQueues.blendItems[&shader].push_back(renderItem);
			}
		}
	}
}

void RenderPipeline::sortEntities(const Camera& camera) {
	const glm::vec3 camPos = camera.position();

	auto sortBatches = [&](auto& batch, bool transparent) {
		for (auto& [shader, renderItems]: batch) {
			std::sort(
				renderItems.begin(),
				renderItems.end(),
				[&camPos, &transparent](const RenderItem& a, const RenderItem& b) {
					const float da = glm::length2(camPos - a.entity->getComponent<TransformComponent>().position);
					const float db = glm::length2(camPos - b.entity->getComponent<TransformComponent>().position);

					if (transparent)
						return da > db;
					return da < db;
				}
			);
		}
	};

	// Sort opaque objects front to back
	sortBatches(renderQueues.deferredItems, false);
	sortBatches(renderQueues.forwardOpaqueItems, false);
	sortBatches(renderQueues.blendItems, true);

	// for (auto& [transforms, materials]: renderQueues.blendInstancedGroup) {
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
