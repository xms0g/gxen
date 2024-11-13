#include "engine.h"
#include <iostream>
#include "glm/glm.hpp"
#include "image/stb_image.h"
#include "glad/glad.h"
#include "window.h"
#include "shader.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "gui.h"
#include "io.hpp"

XEngine::XEngine() = default;

XEngine::~XEngine() = default;

void XEngine::init() {
    mWindow = std::make_unique<Window>();
    mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    mInput = std::make_unique<Input>();

    mWindow->init("X-Engine");
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    mShader = std::make_unique<Shader>(fs::path(SHADER_DIR + "model.vert.glsl"),
                                       fs::path(SHADER_DIR + "model.frag.glsl"));

    mGui = std::make_unique<Gui>(mWindow->nativeHandle(), mWindow->glContext());

    mModel = std::make_unique<Model>(fs::path(ASSET_DIR + "backpack/backpack.obj"));

#ifndef DEBUG
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    std::cout << "OpenGL Driver Vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << '\n';
#endif
}

void XEngine::run() {
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
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

    mShader->activate();
    // view/projection transformations
    glm::mat4 viewMat = mCamera->getViewMatrix();
    glm::mat4 projectionMat = glm::perspective(glm::radians(mCamera->getZoom()),
                                               (float) SCR_WIDTH / (float) SCR_HEIGHT, ZNEAR, ZFAR);
    mShader->setMat4("projection", projectionMat);
    mShader->setMat4("view", viewMat);
    // render the loaded model
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
    mShader->setMat4("model", modelMat);

    mCamera->update();
}

void XEngine::Render() {
    mWindow->clear(0.2f, 0.3f, 0.3f, 1.0f);

//        glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
//        lightPos.x += cos(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
//        lightPos.z += sin(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
    // be sure to activate shader when setting uniforms/drawing objects

    mModel->draw(*mShader);

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
