#include "engine.h"
#include <iostream>
#include "input.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "window.h"
#include "guiSystem.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../renderer/renderSystem.h"
#include "../renderer/lightSystem.h"
#include "../renderer/postProcess.h"
#include "../renderer/skyboxSystem.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Registry* registry) {
	mRegistry = registry;

	mWindow = std::make_unique<Window>();
	mWindow->init("XEngine");

	registry->addSystem<GuiSystem>(mWindow->nativeHandle(), mWindow->glContext());
	mGuiSystem = &registry->getSystem<GuiSystem>();

	registry->addSystem<RenderSystem>();
	mRenderSystem = &registry->getSystem<RenderSystem>();

	registry->addSystem<LightSystem>();
	mLightSystem = &registry->getSystem<LightSystem>();

	mRenderSystem->setLightSystem(mLightSystem);

	registry->addSystem<SkyboxSystem>();
	mSkyboxSystem = &registry->getSystem<SkyboxSystem>();

	mPostProcess = std::make_unique<PostProcess>(mRenderSystem->getSceneWidth(), mRenderSystem->getSceneHeight());

	mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
	mInput = std::make_unique<Input>();
}

void XEngine::configure() const {
	mRenderSystem->configureUB(*mCamera);
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

		mRenderSystem->beginSceneRender();
		mSkyboxSystem->render(*mCamera);
		mRenderSystem->render(*mCamera);
		mRenderSystem->endSceneRender();

		mPostProcess->render(mRenderSystem->getSceneTexture());
#ifdef DEBUG
		mGuiSystem->update(mDeltaTime);
		mGuiSystem->render(mPostProcess->effects());
#endif

		// SDL swap buffers
		mWindow->swapBuffer();
	}
}
