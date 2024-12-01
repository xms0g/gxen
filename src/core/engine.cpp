#include "engine.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "camera.h"
#include "gui.h"
#include "../scene/scene.h"
#include "../renderer/renderer.h"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init() {
    mWindow = std::make_unique<Window>();
    mWindow->init("XEngine");

    mRenderer = std::make_unique<Renderer>();
    mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    mInput = std::make_unique<Input>();

    mGui = std::make_unique<Gui>(mWindow->nativeHandle(), mWindow->glContext());
}

void XEngine::run() {
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void XEngine::addScene(std::unique_ptr<Scene>& scene) {
    mScene = std::move(scene);
}

void XEngine::ProcessInput() {
    mInput->process(*mCamera, mWindow->nativeHandle(), mDeltaTime, isRunning);
}

void XEngine::Update() {
    mDeltaTime = (SDL_GetTicks() - mMillisecsPreviousFrame) / 1000.0f;
    mMillisecsPreviousFrame = SDL_GetTicks();

    updateFpsCounter();
#ifdef DEBUG
    mGui->setFPS(mFPS);
#endif
    mCamera->update();

    for (auto& model: mScene->getModels()) {
        mRenderer->update(*model, mCamera->getViewMatrix(), mCamera->getZoom());
    }


}

void XEngine::Render() {
    mWindow->clear(0.2f, 0.3f, 0.3f, 1.0f);

//        glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
//        lightPos.x += cos(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
//        lightPos.z += sin(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
    // be sure to activate shader when setting uniforms/drawing objects

    for (auto& model: mScene->getModels()) {
        mRenderer->render(*model);
    }
#ifdef DEBUG
    mGui->render();
#endif

    // SDL swap buffers
    mWindow->swapBuffer();
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
