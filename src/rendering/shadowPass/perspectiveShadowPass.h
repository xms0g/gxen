#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "../renderers/renderSystem.h"

class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class PerspectiveShadowPass final : public RenderSystem {
public:
	PerspectiveShadowPass(int mapWidth, int mapHeight);

	~PerspectiveShadowPass() override;

	[[nodiscard]] uint32_t getShadowMap() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& direction, const glm::vec4& position, float fovy);

private:
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
