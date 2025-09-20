#include "scene.h"
#include "glm/glm.hpp"
#include "../core/camera.h"
#include "../renderer/shader.h"
#include "../entity/entity.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void Scene::update(const Camera& camera) {
    rotation += 0.1f;

    for (const auto& entity : entities) {
        Shader& modelShader = entity->getShader();
        modelShader.activate();

        // view/projection transformations
        glm::mat4 projectionMat = glm::perspective(glm::radians(camera.getZoom()),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), ZNEAR, ZFAR);
        modelShader.setMat4("projection", projectionMat);
        modelShader.setMat4("view", camera.getViewMatrix());

        // render the loaded model
        auto modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(rotation), glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
        modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", modelMat);
    }
}

void Scene::addEntity(std::unique_ptr<Entity>& entity) {
    entities.push_back(std::move(entity));
}

std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
    return entities;
}
