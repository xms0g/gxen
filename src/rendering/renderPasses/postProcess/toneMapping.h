#pragma once
#include <memory>
#include "IPostEffect.hpp"

class FrameBuffer;
class Shader;

class ToneMapping final : public IPostEffect {
public:
	float exposure{};

	explicit ToneMapping(const std::string& name, bool enabled = false, float exp = 1.0f);

	uint32_t render(uint32_t sceneTexture, uint32_t VAO,
	                int& toggle, const std::unique_ptr<FrameBuffer>* renderTargets) const override;

private:
	std::unique_ptr<Shader> shader;
};
