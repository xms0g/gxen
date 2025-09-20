#include "renderer.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "window.h"
#include "gui.h"
#include "../config/config.hpp"
#include "../model/model.h"
#include "../entity/entity.hpp"
#include "../scene/scene.h"

Renderer::Renderer(Scene* scene) {
    mScene = scene;
    mWindow = std::make_unique<Window>();
    mWindow->init("XEngine");

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    mGui = std::make_unique<Gui>(mWindow->nativeHandle(), mWindow->glContext());

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::render() const {
    mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

    for (const auto& entity: mScene->getEntities()) {
        entity->shader()->activate();
        entity->draw();
    }

#ifdef DEBUG
    mGui->render();
#endif

    // SDL swap buffers
    mWindow->swapBuffer();

}
