#pragma once

enum class DebugMode {
	None,
	Normals,
	Wireframe
};

class Shader;

struct DebugComponent {
	DebugMode mode{DebugMode::None};

	DebugComponent() = default;

	explicit DebugComponent(const DebugMode m) : mode(m) {
	}
};
