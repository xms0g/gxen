#include "scene.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../core/camera.h"
#include "../renderer/shader.h"
#include "../entity/entity.hpp"

Scene::Scene() {
    mCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 8.0f));
}

void Scene::update() const {
    mCamera->update();

    for (const auto& entity : entities) {
        const Shader* modelShader = entity->getShader();
        modelShader->activate();

        // view/projection transformations
        glm::mat4 projectionMat = glm::perspective(glm::radians(mCamera->getZoom()),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), ZNEAR, ZFAR);
        modelShader->setMat4("projection", projectionMat);
        modelShader->setMat4("view", mCamera->getViewMatrix());

        // render the loaded model
        auto modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, entity->position);
        modelMat = glm::rotate(modelMat, glm::radians(entity->rotation), glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
        modelMat = glm::scale(modelMat, glm::vec3(entity->scale, entity->scale, entity->scale));
        modelShader->setMat4("model", modelMat);
    }
}

void Scene::addEntity(std::unique_ptr<Entity>& entity) {
    entities.push_back(std::move(entity));
}
