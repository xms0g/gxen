#pragma once

#include <cstdint>
#include "glad/glad.h"

class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height);

	~FrameBuffer();

	[[nodiscard]] uint32_t width() const { return mWidth; }
	[[nodiscard]] uint32_t height() const { return mHeight; }
	[[nodiscard]] GLuint texture() const { return mTextureColorBuffer; }

	void bind() const;

	void unbind();

	FrameBuffer& withTexture(bool depthAndStencil = false);

	FrameBuffer& withRenderBuffer();

	FrameBuffer& checkStatus();

private:
	uint32_t mWidth{0};
	uint32_t mHeight{0};
	GLuint mFBO{0};
	GLuint mRB0{0};
	GLuint mDepthStencilTex{0};
	GLuint mTextureColorBuffer{0};
};
