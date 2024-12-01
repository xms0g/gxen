#include "scene.h"

void Scene::addModel(std::unique_ptr<Model>& model) {
    models.push_back(std::move(model));
}

std::vector<std::unique_ptr<Model>>& Scene::getModels() {
    return models;
}
