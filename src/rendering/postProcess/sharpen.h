#pragma once
#include <memory>
#include "IPostEffect.hpp"
#include "quadDraw.hpp"

class FrameBuffer;
class Shader;

class Sharpen final : public IPostEffect, public QuadDraw {
public:
	explicit Sharpen(const std::string& name, bool enabled = false);

	uint32_t render(uint32_t sceneTexture,
	                uint32_t VAO,
	                const std::unique_ptr<FrameBuffer>* renderTargets,
	                int& toggle) const override;

private:
	std::unique_ptr<Shader> shader;
	static constexpr float kernel[9] = {
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	};
};
