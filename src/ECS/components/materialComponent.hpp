#pragma once

#include <memory>

class Shader;
struct MaterialComponent {
	std::shared_ptr<Shader> shader;

	MaterialComponent() = default;
	explicit MaterialComponent(const std::shared_ptr<Shader>& s) : shader(s) {}
};