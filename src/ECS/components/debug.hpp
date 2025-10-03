#pragma once

#include <memory>

enum class DebugMode {
	None,
	Normals
};

class Shader;
struct DebugComponent {
	std::shared_ptr<Shader> shader;
	DebugMode mode{DebugMode::None};

	DebugComponent() = default;
	explicit DebugComponent(const std::shared_ptr<Shader>& s, const DebugMode m): shader(s), mode(m) {}
};