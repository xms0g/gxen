#pragma once
#include <unordered_map>
#include "renderSystem.h"

enum class DebugMode;
class UniformBuffer;

class DebugRenderer final : public RenderSystem {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render() const;

private:
	std::unordered_map<DebugMode, std::shared_ptr<Shader> > mDebugShaders;
};
