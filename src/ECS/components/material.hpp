#pragma once
#include <vector>

struct Texture;

struct MaterialComponent {
	const std::vector<Texture>* textures;
	glm::vec3 color;
	float shininess;
	uint32_t flags;

	MaterialComponent() = default;

	explicit MaterialComponent(const std::vector<Texture>* tex, const float s = 32.0f, const uint32_t f = 1)
		: textures(tex), color(1.0f), shininess(s), flags(f) {
	}

	explicit MaterialComponent(const glm::vec3& c, const float s = 32.0f, const uint32_t f = 1)
		: textures(nullptr), color(c), shininess(s), flags(f) {
	}
};
