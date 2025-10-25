#pragma once
#include <memory>
#include "IPostEffect.hpp"
#include "quadDraw.hpp"

class FrameBuffer;
class Shader;

class Sepia final : public IPostEffect, public QuadDraw {
public:
	explicit Sepia(const std::string& name, bool enabled = false);

	uint32_t render(uint32_t sceneTexture,
	                uint32_t VAO,
	                const std::unique_ptr<FrameBuffer>* renderTargets,
	                int& toggle) const override;

private:
	std::unique_ptr<Shader> shader;
};
