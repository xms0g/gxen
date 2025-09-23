#include "engine.h"
#include <iostream>
#include "input.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "../renderer/window.h"
#include "../config/config.hpp"
#include "../ECS/registry.h"
#include "../renderer/gui.h"
#include "../renderer/renderer.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Registry* registry) {
	mRegistry = registry;

	try {
		registry->addSystem<Renderer>();
		mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 8.0f));
		mInput = std::make_unique<Input>();
	} catch (std::runtime_error& e) {
		std::cerr << e.what() << '\n';
	}
}

void XEngine::run() {
	while (isRunning) {
		mInput->process(mCamera.get(), mRegistry->getSystem<Renderer>().window()->nativeHandle(), mDeltaTime,
		                isRunning);

		mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
		mMillisecsPreviousFrame = SDL_GetTicks();

#ifdef DEBUG
		mRegistry->getSystem<Renderer>().gui()->updateFpsCounter(mDeltaTime);
#endif
		mCamera->update();

		mRegistry->getSystem<Renderer>().render(mCamera.get());
	}
}
