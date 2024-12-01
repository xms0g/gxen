#include "renderer.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "shader.h"
#include "../model/model.h"
#include "../config/config.hpp"

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

void Renderer::update(Model& model, glm::mat4 viewMatrix, float zoom) {
    Shader& modelShader = model.getShader();
    modelShader.activate();
    // view/projection transformations
    glm::mat4 projectionMat = glm::perspective(glm::radians(zoom),
                                               (float) SCR_WIDTH / (float) SCR_HEIGHT, ZNEAR, ZFAR);
    modelShader.setMat4("projection", projectionMat);
    modelShader.setMat4("view", viewMatrix);
    // render the loaded model
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
    modelShader.setMat4("model", modelMat);
}

void Renderer::render(Model& model) {
    model.draw();
}
