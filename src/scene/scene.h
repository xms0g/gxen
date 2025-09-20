#pragma once

#include <vector>

class Entity;
class Camera;
class Scene {
public:
    Scene() = default;

    ~Scene() = default;

    void update(const Camera& camera);

    void addEntity(std::unique_ptr<Entity>& model);

    std::vector<std::unique_ptr<Entity>>& getEntities();

private:
    float rotation{0.0f};
    std::vector<std::unique_ptr<Entity>> entities;
};
