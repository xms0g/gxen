#include "engine.h"
#include <iostream>
#include "input.h"
#include "camera.h"
#include "../config/config.hpp"
#include "../renderer/gui.h"
#include "../renderer/window.h"
#include "../model/model.h"
#include "../scene/scene.h"
#include "../renderer/renderer.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init(Scene* scene) {
    mScene = scene;
    try {
        mRenderer = std::make_unique<Renderer>(scene);
        mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 8.0f));
        mInput = std::make_unique<Input>();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
    }
}

void XEngine::run() {
    while (isRunning) {
        mInput->process(*mCamera, mRenderer->window()->nativeHandle(), mDeltaTime, isRunning);

        mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
        mMillisecsPreviousFrame = SDL_GetTicks();

        updateFpsCounter();

#ifdef DEBUG
        mRenderer->gui()->setFPS(mFPS);
#endif

        mCamera->update();

        mScene->update(*mCamera);

        mRenderer->render();
    }
}

void XEngine::updateFpsCounter() {
    double elapsedSeconds;

    mCurrentFrameCount++;

    mCurrentSeconds += mDeltaTime;
    elapsedSeconds = mCurrentSeconds - mPreviousSeconds;
    // limit text updates to 4 per second
    if (elapsedSeconds > 0.25) {
        mPreviousSeconds = mCurrentSeconds;
        mFPS = mCurrentFrameCount / elapsedSeconds;
        mCurrentFrameCount = 0;
    }
}
