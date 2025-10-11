#pragma once
#include <vector>

struct Texture;

struct MaterialComponent {
	const std::vector<Texture>* textures;
	float shininess;
	uint32_t flags;

	MaterialComponent() = default;

	explicit MaterialComponent(const std::vector<Texture>* tex, const float s = 32.0f, const uint32_t f = 1)
		: textures(tex), shininess(s), flags(f) {
	}
};
