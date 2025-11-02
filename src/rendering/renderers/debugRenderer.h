#pragma once
#include "forwardRenderer.h"

class UniformBuffer;

class DebugRenderer final : public ForwardRenderer {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render(const std::vector<Entity>& entities) const;

private:
	std::array<std::shared_ptr<Shader>, 3> mDebugShaders;
};
