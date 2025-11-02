#include "engine.h"
#include <iostream>
#include "glm/glm.hpp"
#include "input.h"
#include "camera.h"
#include "window.h"
#include "gui/guiSystem.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../rendering/renderPipeline.h"
#include "../rendering/postProcess/postProcess.h"
#include "../rendering/shader.h"

Engine::Engine() = default;

Engine::~Engine() = default;

void Engine::init(Registry* registry) {
	mRegistry = registry;

	mWindow = std::make_unique<Window>();
	mWindow->init("GXen");

	registry->addSystem<GuiSystem>(mWindow->nativeHandle(), mWindow->glContext());
	mGuiSystem = &registry->getSystem<GuiSystem>();

	registry->addSystem<RenderPipeline>(registry);
	mRenderPipeline = &registry->getSystem<RenderPipeline>();

	mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
	mInput = std::make_unique<Input>();
}

void Engine::configure() const {
	mRenderPipeline->batchEntities(*mCamera);
	mRenderPipeline->configure(*mCamera);
}

void Engine::run() {
	while (isRunning) {
		mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

		mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
		mMillisecsPreviousFrame = SDL_GetTicks();

		mInput->process(*mCamera, mWindow->nativeHandle(), mDeltaTime, isRunning);
		mCamera->update();
		mRenderPipeline->render(*mCamera);
#ifdef DEBUG
		mGuiSystem->update(mDeltaTime);
		mGuiSystem->render(mRenderPipeline->postProcess().effects());
#endif
		// SDL swap buffers
		mWindow->swapBuffer();
	}
}
