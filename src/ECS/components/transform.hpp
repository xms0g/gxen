#pragma once

#include "glm/glm.hpp"

struct TransformComponent {
	glm::vec3 position{};
	glm::vec3 scale{};
	double rotation;

	TransformComponent() = default;
	explicit TransformComponent(const glm::vec3 position, const glm::vec3 scale,
	                            const double rotation) : position(position),
	                                                     scale(scale),
	                                                     rotation(rotation) {
	}
};
