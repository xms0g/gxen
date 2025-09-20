#include "renderer.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "shader.h"
#include "../model/model.h"
#include "../light/light.h"
#include "../entity/entity.hpp"

Renderer::Renderer() {
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
}

void Renderer::render(std::unique_ptr<Entity>& entity) {
    entity->draw();
}
