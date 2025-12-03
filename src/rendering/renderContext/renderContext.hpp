#pragma once
#include <array>

class LightSystem;
class FrameBuffer;
class UniformBuffer;
struct RenderQueue;

struct RenderContext {
	const RenderQueue* renderQueue;
	const LightSystem* lightSystem;
	const FrameBuffer* sceneBuffer;
	const UniformBuffer* cameraUBO;
	const UniformBuffer* shadowMapUBO;

	struct {
		int width, height;
	} screen;

	struct {
		uint32_t textureSlot;
		const std::array<uint32_t, 3>* textures;
	} shadowMap;
};