#pragma once
#include <memory>

class Shader;
struct ShaderComponent {
	std::shared_ptr<Shader> shader;

	ShaderComponent() = default;
	explicit ShaderComponent(const std::shared_ptr<Shader>& s) : shader(s) {}
};