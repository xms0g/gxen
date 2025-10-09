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
#include "../rendering/renderers/forwardRenderer.h"
#include "../rendering/renderers/debugRenderer.h"
#include "../rendering/lightSystem.h"
#include "../rendering/postProcess.h"
#include "../rendering/skyboxSystem.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Registry* registry) {
	mRegistry = registry;

	mWindow = std::make_unique<Window>();
	mWindow->init("XEngine");

	registry->addSystem<GuiSystem>(mWindow->nativeHandle(), mWindow->glContext());
	mGuiSystem = &registry->getSystem<GuiSystem>();

	registry->addSystem<ForwardRenderer>();
	mForwardRenderer = &registry->getSystem<ForwardRenderer>();

	registry->addSystem<DebugRenderer>();
	mDebugRenderer = &registry->getSystem<DebugRenderer>();

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	mForwardRenderer->setLightSystem(mLightSystem);

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();

	mPostProcess = std::make_unique<PostProcess>(mForwardRenderer->getSceneWidth(), mForwardRenderer->getSceneHeight());

	mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
	mInput = std::make_unique<Input>();
}

void XEngine::configure() const {
	mForwardRenderer->configure(*mCamera);
	mDebugRenderer->configure(mForwardRenderer->getCameraUBO());
}

void XEngine::run() {
	while (isRunning) {
		mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

		mInput->process(*mCamera, mWindow->nativeHandle(), mDeltaTime,
		                isRunning);

		mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
		mMillisecsPreviousFrame = SDL_GetTicks();

		mCamera->update();
		mLightSystem->update();
		mForwardRenderer->updateBuffers(*mCamera);

		mForwardRenderer->batchEntities(*mCamera);
		mForwardRenderer->shadowPass();

		mForwardRenderer->beginSceneRender();
		mSkyboxSystem->render(*mCamera);
		mForwardRenderer->opaquePass();
		mForwardRenderer->instancedPass();
		mDebugRenderer->render();
		mForwardRenderer->transparentPass();
		mForwardRenderer->transparentInstancedPass(*mCamera);
		mForwardRenderer->endSceneRender();

		mPostProcess->render(mForwardRenderer->getIntermediateTexture());
#ifdef DEBUG
		mGuiSystem->update(mDeltaTime);
		mGuiSystem->render(mPostProcess->effects());
#endif

		// SDL swap buffers
		mWindow->swapBuffer();
	}
}
