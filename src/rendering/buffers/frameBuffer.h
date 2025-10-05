#pragma once

#include "glad/glad.h"

class FrameBuffer {
public:
	FrameBuffer(int width, int height);

	~FrameBuffer();

	[[nodiscard]] int width() const { return mWidth; }
	[[nodiscard]] int height() const { return mHeight; }
	[[nodiscard]] GLuint texture() const { return mTextureColorBuffer; }

	void bind() const;

	void unbind();

	FrameBuffer& withTexture();

	FrameBuffer& withTextureMultisampled();

	FrameBuffer& withTextureDepthStencil();

	FrameBuffer& withTextureDepthStencilMultisampled();

	FrameBuffer& withRenderBufferDepthStencil();

	FrameBuffer& withRenderBufferDepthStencilMultisampled();

	void checkStatus();

private:
	int mWidth{0};
	int mHeight{0};
	GLuint mFBO{0};
	GLuint mRB0{0};
	GLuint mDepthStencilTex{0};
	GLuint mTextureColorBuffer{0};
};
