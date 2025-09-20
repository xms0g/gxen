#pragma once

#include "glm/glm.hpp"

class Shader;
struct Entity {
    float rotation{0.0f};
    float scale{1.0f};
    glm::vec3 position{0.0f};

    virtual ~Entity() = default;
    [[nodiscard]] virtual Shader* shader() const = 0;
    virtual void draw() const = 0;
};