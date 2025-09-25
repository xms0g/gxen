#pragma once

#include <vector>

struct Texture;
struct MaterialComponent {
	const std::vector<Texture>* textures;
	float shininess;

	MaterialComponent() = default;

	explicit MaterialComponent(
		const std::vector<Texture>* t,
		const float s) : textures(t), shininess(s) {}
};