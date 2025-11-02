#pragma once
#include <unordered_map>
#include "forwardRenderer.h"

enum class DebugMode;
class UniformBuffer;

class DebugRenderer final : public ForwardRenderer {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render(std::vector<Entity>& entities) const;

private:
	std::unordered_map<DebugMode, std::shared_ptr<Shader> > mDebugShaders;
};
