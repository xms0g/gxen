#pragma once

#include "glm/glm.hpp"

class Shader;
class Entity {
public:
	virtual ~Entity() = default;
	[[nodiscard]] virtual Shader* shader() const = 0;
	virtual void draw() const = 0;

    float rotation{0.0f};
    float scale{1.0f};
    glm::vec3 position{0.0f};
};