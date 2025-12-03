#pragma once

struct Material;
struct Texture;

struct MaterialComponent {
	using MaterialMap = std::unordered_map<uint32_t, Material>;
	const MaterialMap* materials;

	glm::vec3 color;
	float shininess;
	float heightScale;
	uint32_t flag;

	MaterialComponent() = default;

	explicit MaterialComponent(const MaterialMap* mat,
	                           const float s = 32.0f,
	                           const float h = 1.0f,
	                           const uint32_t f = 1 << 1)
		: materials(mat), color(1.0f), shininess(s), heightScale(h), flag(f) {
	}

	explicit MaterialComponent(const glm::vec3& c,
	                           const float s = 32.0f,
	                           const float h = 1.0f,
	                           const uint32_t f = 1 << 1)
		: materials(nullptr), color(c), shininess(s), heightScale(h), flag(f) {
	}
};
