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

	~DirectionalShadowPass() override;

	[[nodiscard]] uint32_t getShadowMap() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const glm::vec3& dir);

private:
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
