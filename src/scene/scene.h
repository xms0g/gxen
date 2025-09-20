#pragma once
#include <vector>

class Entity;
class Camera;
class Scene {
public:
    Scene() = default;

    ~Scene() = default;

    void update(const Camera& camera, float rt);

    void addEntity(std::unique_ptr<Entity>& model);

    std::vector<std::unique_ptr<Entity>>& getEntities();

private:
    std::vector<std::unique_ptr<Entity>> entities;
};
