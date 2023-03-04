#include "Engine.h"
#include <iostream>
#include "image/stb_image.h"
#include "filesystem/filesystem.h"
#include "glad/glad.h"
#include "Shader.h"
#include "Model.h"
#include "Input.h"
#include "Camera.h"
#include "Configs.hpp"


Engine::Engine() :
        camera(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f))),
        input(std::make_unique<Input>()) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL" << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window = SDL_CreateWindow(
            "OpenGL Test",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            displayMode.w,
            displayMode.h,
            SDL_WINDOW_OPENGL);

    if (!window) {
        std::cerr << "Error creating SDL Window";
        return;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

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

    shader = std::make_unique<Shader>(Filesystem::path(SHADER_DIR + "model.vert.glsl"),
                                      Filesystem::path(SHADER_DIR + "model.frag.glsl"));
    model = std::make_unique<Model>(Filesystem::path(ASSET_DIR + "backpack/backpack.obj"));

    isRunning = true;

#ifdef DEBUG
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	std::cout << "OpenGL Driver Vendor: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << '\n';
#endif
}

Engine::~Engine() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


bool Engine::IsRunning() const {
    return isRunning;
}


void Engine::ProcessInput() {
    input->Process(*camera, deltaTime, isRunning);
}


void Engine::Update() {
    deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0f;
    millisecsPreviousFrame = SDL_GetTicks();

    shader->Activate();
    // view/projection transformations
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()),
                                            (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    shader->SetMat4("projection", projection);
    shader->SetMat4("view", view);
    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
    shader->SetMat4("model", model);
}


void Engine::Render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
//        lightPos.x += cos(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
//        lightPos.z += sin(static_cast<float>(SDL_GetTicks() / 1000.0)) * 2.0f;
    // be sure to activate shader when setting uniforms/drawing objects

    model->Draw(*shader);

    // SDL swap buffers
    SDL_GL_SwapWindow(window);
}
