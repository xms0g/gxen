#pragma once

#include <vector>
#include "glm/glm.hpp"

struct Texture;

struct MaterialComponent {
	const std::vector<Texture>* textures;
	float shininess;
	glm::vec3 color;
	uint32_t flags;

	MaterialComponent() = default;

	explicit MaterialComponent(const std::vector<Texture>* tex, const float s = 32.0f, const uint32_t f = 1)
		: textures(tex), shininess(s), color(1.0f), flags(f) {
	}

	explicit MaterialComponent(const glm::vec3& c, const float s = 32.0f, const uint32_t f = 1)
		: textures(nullptr), shininess(s), color(c), flags(f) {
	}
};
