#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"

class ForwardRenderer;
class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class PerspectiveShadowPass {
public:
	PerspectiveShadowPass(const ForwardRenderer& fr, int mapWidth, int mapHeight);

	~PerspectiveShadowPass();

	[[nodiscard]] uint32_t getShadowMap() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& direction, const glm::vec4& position, float fovy);

private:
	const ForwardRenderer& mForwardRenderer;
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
