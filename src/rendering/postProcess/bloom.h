#pragma once
#include "IPostEffect.hpp"

namespace Models {
class Quad;
}

class Shader;
class FrameBuffer;

class Bloom final : public IPostEffect {
public:
	Bloom(const std::string& name, int width, int height, bool enabled = false);

	uint32_t render(uint32_t sceneTexture, uint32_t VAO) const;

private:
	void draw(uint32_t sceneTexture, uint32_t VAO) const;
	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::unique_ptr<Shader> brightFilter;
	std::unique_ptr<Shader> blur;
	std::unique_ptr<Shader> combine;
};
