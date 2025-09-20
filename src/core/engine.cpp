#include "engine.h"
#include <iostream>
#include "input.h"
#include "../renderer/window.h"
#include "../config/config.hpp"
#include "../renderer/gui.h"
#include "../scene/scene.h"
#include "../renderer/renderer.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Scene* scene) {
    mScene = scene;
    try {
        mRenderer = std::make_unique<Renderer>(scene);
        mInput = std::make_unique<Input>();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
    }
}

void XEngine::run() {
    while (isRunning) {
        mInput->process(mScene->camera(), mRenderer->window()->nativeHandle(), mDeltaTime, isRunning);

        mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
        mMillisecsPreviousFrame = SDL_GetTicks();

#ifdef DEBUG
        mRenderer->gui()->updateFpsCounter(mDeltaTime);
#endif

        mScene->update();

        mRenderer->render();
    }
}