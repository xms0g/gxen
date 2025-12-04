#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"

struct RenderContext;
class Shader;
class FrameBuffer;

class OmnidirectionalShadowPass {
public:
	explicit OmnidirectionalShadowPass(const RenderContext& context);

	~OmnidirectionalShadowPass();

	[[nodiscard]] uint32_t getDepthTexture() const;

	[[nodiscard]] FrameBuffer& getDepthMap() const;

	void render(const RenderContext& context, const glm::vec4& position, int layer) const;

private:
	std::unique_ptr<FrameBuffer> mDepthMap;
	std::unique_ptr<Shader> mDepthShader;
};
