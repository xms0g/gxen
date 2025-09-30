#pragma once

#include <vector>
#include "glm/glm.hpp"

struct Texture;

struct MaterialComponent {
	const std::vector<Texture>* textures;
	float shininess;
	glm::vec3 color;
	bool isTransparent;

	MaterialComponent() = default;

	explicit MaterialComponent(const std::vector<Texture>* tex, const float s = 32.0f, const bool t = false)
		: textures(tex), shininess(s), color(1.0f), isTransparent(t) {
	}

	explicit MaterialComponent(const glm::vec3& c, const float s = 32.0f, const bool t = false)
		: textures(nullptr), shininess(s), color(c), isTransparent(t) {
	}
};
