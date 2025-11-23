#pragma once
#include <memory>
#include <vector>
#include <array>

class Shader;
struct RenderItem;
class UniformBuffer;

class DebugRenderer {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render(const std::vector<RenderItem>& renderItems) const;

private:
	std::array<std::shared_ptr<Shader>, 3> mDebugShaders;
};
