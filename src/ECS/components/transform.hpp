#pragma once

#include "glm/glm.hpp"

struct TransformComponent {
	glm::vec3 position{};
	glm::vec3 scale{};
	float rotation{};

	TransformComponent() = default;
	explicit TransformComponent(const glm::vec3 position, const glm::vec3 scale,
	                            const float rotation) : position(position),
	                                                     scale(scale),
	                                                     rotation(rotation) {
	}
};
