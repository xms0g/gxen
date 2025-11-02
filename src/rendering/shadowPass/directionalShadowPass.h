#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "../renderers/forwardRenderer.h"

class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class DirectionalShadowPass final : public ForwardRenderer {
public:
	DirectionalShadowPass(int mapWidth, int mapHeight);

	~DirectionalShadowPass();

	[[nodiscard]] uint32_t getShadowMap() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& direction);

private:
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
