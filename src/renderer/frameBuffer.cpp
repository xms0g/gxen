#include "frameBuffer.h"
#include "glad/glad.h"
#include "../config/config.hpp"

FrameBuffer::FrameBuffer(const uint32_t width, const uint32_t height): mWidth(width), mHeight(height) {
	glGenFramebuffers(1, &mFBO);
	bind();
}

FrameBuffer::~FrameBuffer() {
	if (mTextureColorBuffer)
		glDeleteTextures(1, &mTextureColorBuffer);
	if (mDepthStencilTex)
		glDeleteTextures(1, &mDepthStencilTex);
	if (mRB0)
		glDeleteRenderbuffers(1, &mRB0);
	glDeleteFramebuffers(1, &mFBO);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer& FrameBuffer::withTexture(const bool depthAndStencil) {
	glGenTextures(1, &mTextureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, mTextureColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer, 0);

	if (depthAndStencil) {
		// depth and stencil texture
		glGenTextures(1, &mDepthStencilTex);
		glBindTexture(GL_TEXTURE_2D, mDepthStencilTex);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTex, 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withRenderBuffer() {
	glGenRenderbuffers(1, &mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, mRB0);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return *this;
}

FrameBuffer& FrameBuffer::checkStatus() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("ERROR::FRAMEBUFFER::NOT_COMPLETE!\n");
	}
	unbind();
	return *this;
}
