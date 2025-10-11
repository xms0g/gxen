#pragma once
#include <memory>
#include "../renderers/renderSystem.h"

class LightSystem;
class UniformBuffer;
class Entity;
class Shader;
class FrameBuffer;
struct PointLightComponent;

class OmnidirectionalShadowPass final : public RenderSystem {
public:
	OmnidirectionalShadowPass(int mapWidth, int mapHeight);

	~OmnidirectionalShadowPass() override;

	uint32_t getShadowMap() const;

	void render(const std::vector<Entity>& entities, const LightSystem& lights) const;

private:
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
