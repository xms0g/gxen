#pragma once
#include "IPostEffect.hpp"
#include "quadDraw.hpp"

namespace Models {
class Quad;
}

class Shader;
class FrameBuffer;

class Bloom final : public IPostEffect, public QuadDraw {
public:
	Bloom(const std::string& name, int width, int height, bool enabled = false);

	uint32_t render(uint32_t sceneTexture,
	                uint32_t VAO,
	                const std::unique_ptr<FrameBuffer>* renderTargets,
	                int& toggle) const override;

private:
	uint32_t brightFilterPass(uint32_t sceneTexture, uint32_t VAO, int& toggle) const;

	uint32_t blurPass(uint32_t sceneTexture, uint32_t VAO, int& toggle) const;

	uint32_t combinePass(uint32_t sceneTexture, uint32_t bloomBlur, uint32_t VAO, const int& toggle) const;

	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::unique_ptr<Shader> brightFilter;
	std::unique_ptr<Shader> blur;
	std::unique_ptr<Shader> combine;
};
