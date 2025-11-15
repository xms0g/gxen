#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "../../config/config.hpp"

class ForwardRenderer;
class Entity;
class Shader;
class LightSystem;
class FrameBuffer;

class PerspectiveShadowPass {
public:
	PerspectiveShadowPass(int mapWidth, int mapHeight);

	~PerspectiveShadowPass();

	[[nodiscard]] uint32_t getDepthTexture() const;

	[[nodiscard]] glm::mat4 getLightSpaceMatrix(int layer) const;

	void render(const std::vector<Entity>& entities, const glm::vec4& direction,
	            const glm::vec4& position, float fovy, int layer);

private:
	glm::mat4 mLightSpaceMatrix[MAX_SPOT_LIGHTS]{};
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
