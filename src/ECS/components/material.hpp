#pragma once
#include "glm/glm.hpp"

struct Texture;

struct MaterialComponent {
	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture> >;
	const TextureMap* textures;

	glm::vec3 color;
	float shininess;
	float heightScale;
	uint32_t flags;

	MaterialComponent() = default;

	explicit MaterialComponent(
		const TextureMap* tex,
		const float s = 32.0f,
		const float h = 1.0f,
		const uint32_t f = 1)
		: textures(tex), color(1.0f), shininess(s), heightScale(h), flags(f) {
	}

	explicit MaterialComponent(
		const glm::vec3& c,
		const float s = 32.0f,
		const float h = 1.0f,
		const uint32_t f = 1)
		: textures(nullptr), color(c), shininess(s),heightScale(h), flags(f) {
	}
};
