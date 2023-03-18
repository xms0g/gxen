#include "Engine.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"
#include "image/stb_image.h"
#include "filesystem/filesystem.h"
#include "glad/glad.h"
#include "SDLWindow.h"
#include "Configs.hpp"


Engine::Engine() :
        window(std::make_unique<SDLWindow>("OpenGL Test")),
        camera(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f))),
        input(std::make_unique<Input>()) {

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

    shader = std::make_unique<Shader>(Filesystem::path(SHADER_DIR + "model.vert.glsl"),
                                      Filesystem::path(SHADER_DIR + "model.frag.glsl"));

    gui = std::make_unique<Gui>(dynamic_cast<SDLWindow*>(window.get())->GetWindow(),
                                dynamic_cast<SDLWindow*>(window.get())->GetContext());

    model = std::make_unique<Model>(Filesystem::path(ASSET_DIR + "backpack/backpack.obj"));

    isRunning = true;

#ifndef DEBUG
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    std::cout << "OpenGL Driver Vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << '\n';
#endif
}


bool Engine::IsRunning() const {
    return isRunning;
}


void Engine::ProcessInput() {
    input->Process(*camera, dynamic_cast<SDLWindow*>(window.get())->GetWindow(), deltaTime, isRunning);
}


void Engine::Update() {
    deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0f;
    millisecsPreviousFrame = SDL_GetTicks();

    window->UpdateFpsCounter(deltaTime);

    shader->Activate();
    // view/projection transformations
    glm::mat4 viewMat = camera->GetViewMatrix();
    glm::mat4 projectionMat = glm::perspective(glm::radians(camera->GetZoom()),
                                            (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    shader->SetMat4("projection", projectionMat);
    shader->SetMat4("view", viewMat);
    // render the loaded model
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
    shader->SetMat4("model", modelMat);
}


void Engine::Render() {
    window->Clear();

//        glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
//        lightPos.x += cos(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
//        lightPos.z += sin(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
    // be sure to activate shader when setting uniforms/drawing objects

    model->Draw(*shader);

    gui->Render();

    // SDL swap buffers
    window->SwapBuffer();
}
