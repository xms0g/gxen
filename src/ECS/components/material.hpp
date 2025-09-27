#pragma once

#include <vector>

struct Texture;

struct MaterialComponent {
	const std::vector<Texture>* textures;
	float shininess;
	glm::vec3 color;

	MaterialComponent() = default;

	explicit MaterialComponent(const std::vector<Texture>* t, const float s = 32.0f)
		: textures(t), shininess(s), color(1.0f) {
	}

	explicit MaterialComponent(const glm::vec3& c, const float s = 32.0f)
		: textures(nullptr), shininess(s), color(c) {
	}
};
