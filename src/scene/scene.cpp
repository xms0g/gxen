#include "scene.h"
#include "../entity/entity.hpp"

void Scene::addEntity(std::unique_ptr<Entity>& entity) {
    entities.push_back(std::move(entity));
}

std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
    return entities;
}
