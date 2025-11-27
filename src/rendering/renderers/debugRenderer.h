#pragma once
#include <memory>
#include <vector>
#include <array>

struct RenderGroup;
class Shader;
class UniformBuffer;

class DebugRenderer {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render(const std::vector<RenderGroup>& groups) const;

private:
	std::array<std::shared_ptr<Shader>, 3> mDebugShaders;
};
