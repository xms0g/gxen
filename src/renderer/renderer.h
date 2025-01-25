#pragma once

#include <memory>
#include "glm/glm.hpp"
#include "../core/camera.h"

class Entity;
class Camera;
class Renderer {
public:
    Renderer();

    void update(std::unique_ptr<Entity>& entity, Camera& camera, float rt);

    void render(std::unique_ptr<Entity>& entity);
};
