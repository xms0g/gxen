#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "../renderers/renderSystem.h"

class LightSystem;
class UniformBuffer;
class Entity;
class Shader;
class FrameBuffer;

class OmnidirectionalShadowPass final : public RenderSystem {
public:
	OmnidirectionalShadowPass(int mapWidth, int mapHeight);

	~OmnidirectionalShadowPass() override;

	[[nodiscard]] uint32_t getShadowMap() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& position) const;

private:
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
