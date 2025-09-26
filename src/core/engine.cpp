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

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Registry* registry) {
	mRegistry = registry;

	try {
		mWindow = std::make_unique<Window>();
		mWindow->init("XEngine");

		registry->addSystem<GuiSystem>(mWindow->nativeHandle(), mWindow->glContext());
		registry->addSystem<RenderSystem>();
		registry->addSystem<LightSystem>();
		mRegistry->getSystem<RenderSystem>().setLightSystem(&mRegistry->getSystem<LightSystem>());


		mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 8.0f));
		mInput = std::make_unique<Input>();

	} catch (std::runtime_error& e) {
		std::cerr << e.what() << '\n';
	}
}

void XEngine::run() {
	while (isRunning) {
		mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

		mInput->process(mCamera.get(), mWindow->nativeHandle(), mDeltaTime,
		                isRunning);

		mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
		mMillisecsPreviousFrame = SDL_GetTicks();

		mCamera->update();
		mRegistry->getSystem<LightSystem>().update();
		mRegistry->getSystem<RenderSystem>().render(mCamera.get());

#ifdef DEBUG
		mRegistry->getSystem<GuiSystem>().update(mDeltaTime);
		mRegistry->getSystem<GuiSystem>().render();
#endif

		// SDL swap buffers
		mWindow->swapBuffer();
	}
}
