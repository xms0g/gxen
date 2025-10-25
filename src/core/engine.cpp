#include "engine.h"
#include <iostream>
#include "glm/glm.hpp"
#include "input.h"
#include "camera.h"
#include "window.h"
#include "gui/guiSystem.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../rendering/shader.h"
#include "../rendering/buffers/uniformBuffer.h"
#include "../rendering/renderers/forwardRenderer.h"
#include "../rendering/renderers/debugRenderer.h"
#include "../rendering/lightSystem.h"
#include "../rendering/postProcess/postProcess.h"
#include "../rendering/skyboxSystem.h"
#include "../rendering/shadowPass/shadowSystem.h"

Engine::Engine() = default;

Engine::~Engine() = default;

void Engine::init(Registry* registry) {
	mRegistry = registry;

	mWindow = std::make_unique<Window>();
	mWindow->init("GXen");

	registry->addSystem<GuiSystem>(mWindow->nativeHandle(), mWindow->glContext());
	mGuiSystem = &registry->getSystem<GuiSystem>();

	registry->addSystem<ForwardRenderer>();
	mForwardRenderer = &registry->getSystem<ForwardRenderer>();

	registry->addSystem<DebugRenderer>();
	mDebugRenderer = &registry->getSystem<DebugRenderer>();

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();

	registry->addSystem<ShadowSystem>();
	mShadowSystem = &registry->getSystem<ShadowSystem>();

	mPostProcess = std::make_unique<PostProcess>(mForwardRenderer->getSceneWidth(), mForwardRenderer->getSceneHeight());

	mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
	mInput = std::make_unique<Input>();
}

void Engine::configure() const {
	mForwardRenderer->configure(*mCamera, mLightSystem->getLightUBO());
	mDebugRenderer->configure(mForwardRenderer->getCameraUBO());
	mShadowSystem->configure();
}

void Engine::run() {
	while (isRunning) {
		mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

		mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
		mMillisecsPreviousFrame = SDL_GetTicks();

		mInput->process(*mCamera, mWindow->nativeHandle(), mDeltaTime, isRunning);

		preProcess();
		render();
		postProcess();

		// SDL swap buffers
		mWindow->swapBuffer();
	}
}

void Engine::preProcess() const {
	mCamera->update();
	mLightSystem->update();
	mShadowSystem->shadowPass(*mLightSystem);
	mForwardRenderer->updateBuffers(*mCamera);
	mForwardRenderer->batchEntities(*mCamera);
}

void Engine::render() const {
	mForwardRenderer->beginSceneRender();
	mSkyboxSystem->render(*mCamera);
	mForwardRenderer->opaquePass(mShadowSystem->getShadowMaps());
	mForwardRenderer->instancedPass();
	mDebugRenderer->render();
	mForwardRenderer->transparentPass();
	mForwardRenderer->transparentInstancedPass(*mCamera);
	mForwardRenderer->endSceneRender();
}

void Engine::postProcess() const {
	mPostProcess->render(mForwardRenderer->getIntermediateTexture());
#ifdef DEBUG
	mGuiSystem->update(mDeltaTime);
	mGuiSystem->render(mPostProcess->effects());
#endif
}
