#pragma once

#include "renderSystem.h"

class UniformBuffer;

class DebugRenderer final : public RenderSystem {
public:
	DebugRenderer();

	void configure(const UniformBuffer& cameraUBO) const;

	void render() const;
};
