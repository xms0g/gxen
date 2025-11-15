#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"

class ForwardRenderer;
class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class DirectionalShadowPass {
public:
	DirectionalShadowPass(int mapWidth, int mapHeight);

	~DirectionalShadowPass();

	[[nodiscard]] uint32_t getDepthTexture() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& direction);

private:
	glm::mat4 mLightSpaceMatrix{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
