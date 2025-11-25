#pragma once

struct Material;
struct Texture;
enum class RenderFlags : uint32_t;

struct MaterialComponent {
	using MaterialMap = std::unordered_map<uint32_t, Material >;
	const MaterialMap* materials;

	glm::vec3 color;
	float shininess;
	float heightScale;
	RenderFlags flag;

	MaterialComponent() = default;

	explicit MaterialComponent(
		const MaterialMap* mat,
		const float s,
		const float h,
		const RenderFlags f)
		: materials(mat), color(1.0f), shininess(s), heightScale(h), flag(f) {
	}

	explicit MaterialComponent(
		const glm::vec3& c,
		const float s,
		const float h,
		const RenderFlags f)
		: materials(nullptr), color(c), shininess(s),heightScale(h), flag(f) {
	}
};
