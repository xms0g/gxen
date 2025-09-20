#pragma once

#include <vector>

struct Entity;
class Camera;
class Scene {
public:
    Scene();

    ~Scene() = default;

    [[nodiscard]] Camera* camera() const { return mCamera.get(); }

    void update() const;

    void addEntity(std::unique_ptr<Entity>& entity);

    std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }

private:
    std::unique_ptr<Camera> mCamera;
    std::vector<std::unique_ptr<Entity>> entities;
};
