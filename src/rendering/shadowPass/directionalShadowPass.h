#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "../renderers/renderSystem.h"

class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class DirectionalShadowPass final : public RenderSystem {
public:
	DirectionalShadowPass(int mapWidth, int mapHeight);

	~DirectionalShadowPass();

	uint32_t getShadowMap() const;

	glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const LightSystem& lights);

private:
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
