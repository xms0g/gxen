#pragma once

struct MaterialComponent {
	float shininess;

	MaterialComponent() = default;

	explicit MaterialComponent(const float s) : shininess(s) {}
};