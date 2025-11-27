#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"

struct RenderGroup;
class Entity;
class Shader;
class FrameBuffer;

class OmnidirectionalShadowPass {
public:
	OmnidirectionalShadowPass(int mapWidth, int mapHeight);

	~OmnidirectionalShadowPass();

	[[nodiscard]] uint32_t getDepthTexture() const;

	[[nodiscard]] FrameBuffer& getDepthMap() const;

	void render(const std::vector<RenderGroup>& shadowCasters, const glm::vec4& position, int layer) const;

private:
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
