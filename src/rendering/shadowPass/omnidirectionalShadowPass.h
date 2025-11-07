#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"

class ForwardRenderer;
class LightSystem;
class UniformBuffer;
class Entity;
class Shader;
class FrameBuffer;

class OmnidirectionalShadowPass {
public:
	OmnidirectionalShadowPass(int mapWidth, int mapHeight);

	~OmnidirectionalShadowPass();

	[[nodiscard]] uint32_t getShadowMap() const;

	void render(const std::vector<Entity>& entities, const glm::vec4& position) const;

private:
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
