#pragma once

#include <memory>
#include "glad/glad.h"

class FrameBuffer;

namespace Models {
class Quad;
}

class Shader;

class PostProcess {
public:
	PostProcess(uint32_t width, uint32_t height);

	bool& grayScaleEnabled() { return mGrayScaleEnabled; }
	bool& inverseEnabled() { return mInverseEnabled; }
	bool& gammaEnabled() { return mGammaEnabled; }

	void render(GLuint sceneTexture) const;

private:
	void draw(GLuint sceneTexture) const;

	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<Shader> mInverse;
	std::unique_ptr<Shader> mGrayScale;
	std::unique_ptr<Shader> mGamma;

	std::unique_ptr<FrameBuffer> pingPongBuffers[2];

	bool mGrayScaleEnabled{false};
	bool mInverseEnabled{false};
	bool mGammaEnabled{false};
};
