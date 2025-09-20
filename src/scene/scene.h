#pragma once

#include <vector>

class Entity;
class Camera;
class Scene {
public:
    Scene();

    ~Scene() = default;

    Camera* camera() const { return mCamera.get(); }

    void update();

    void addEntity(std::unique_ptr<Entity>& model);

    std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }

private:
    float rotation{0.0f};
    std::unique_ptr<Camera> mCamera;
    std::vector<std::unique_ptr<Entity>> entities;
};
