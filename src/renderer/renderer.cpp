#include "renderer.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "shader.h"
#include "../core/camera.h"
#include "../model/model.h"
#include "../light/light.h"
#include "../entity/entity.hpp"
#include "../config/config.hpp"
#include "../core/camera.h"

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

void Renderer::update(std::unique_ptr<Entity>& entity, Camera& camera, float rt) {
    if (dynamic_cast<Model*>(entity.get())) {
        Shader& modelShader = entity->getShader();
        modelShader.activate();
        // view/projection transformations
        glm::mat4 projectionMat = glm::perspective(
                glm::radians(camera.getZoom()),
                (float) SCR_WIDTH / (float) SCR_HEIGHT, ZNEAR, ZFAR);
        modelShader.setMat4("projection", projectionMat);
        modelShader.setMat4("view", camera.getViewMatrix());
        // render the loaded model
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(rt), glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
        modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", modelMat);
    } else if (auto light = dynamic_cast<Light*>(entity.get())) {
        Shader& shader = entity->getShader();
        shader.activate();

        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", light->pos());
        shader.setVec3("viewPos", camera.getPosition());
    }

}

void Renderer::render(std::unique_ptr<Entity>& entity) {
    entity->draw();
}
