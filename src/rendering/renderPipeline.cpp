#include "renderPipeline.h"
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

RenderPipeline::RenderPipeline(Registry* registry) {
	registry->addSystem<ForwardRenderer>();
	mForwardRenderer = &registry->getSystem<ForwardRenderer>();

	registry->addSystem<DeferredRenderer>();
	mDeferredRenderer = &registry->getSystem<DeferredRenderer>();

	registry->addSystem<DebugRenderer>();
	mDebugRenderer = &registry->getSystem<DebugRenderer>();

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();

	registry->addSystem<ShadowSystem>();
	mShadowSystem = &registry->getSystem<ShadowSystem>();

	mPostProcess = std::make_unique<PostProcess>(mForwardRenderer->getSceneWidth(), mForwardRenderer->getSceneHeight());
}

RenderPipeline::~RenderPipeline() = default;

PostProcess& RenderPipeline::postProcess() const { return *mPostProcess; }

void RenderPipeline::configure(const Camera& camera) const {
	mForwardRenderer->configure(camera, mLightSystem->getLightUBO());
	mDeferredRenderer->configure(mLightSystem->getLightUBO());
	mDebugRenderer->configure(mForwardRenderer->getCameraUBO());
	mShadowSystem->configure();
}

void RenderPipeline::render(const Camera& camera) const {
	mLightSystem->update();
	mShadowSystem->shadowPass(*mLightSystem);
	mForwardRenderer->updateBuffers(camera);
	mForwardRenderer->batchEntities(camera);

	mForwardRenderer->beginSceneRender();
	mSkyboxSystem->render(camera);
	//mDeferredRenderer->geometryPass();
	//mDeferredRenderer->lightingPass(mShadowSystem->getShadowMaps());
	mForwardRenderer->opaquePass(mShadowSystem->getShadowMaps());
	mForwardRenderer->instancedPass();
	mDebugRenderer->render();
	mForwardRenderer->transparentPass();
	mForwardRenderer->transparentInstancedPass(camera);
	mForwardRenderer->endSceneRender();

	mPostProcess->render(mForwardRenderer->getIntermediateTexture());
}
