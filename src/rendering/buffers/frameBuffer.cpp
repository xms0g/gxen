#include "frameBuffer.h"
#include "glad/glad.h"
#include "../../config/config.hpp"

FrameBuffer::FrameBuffer(const int width, const int height) : mWidth(width), mHeight(height) {
	glGenFramebuffers(1, &mFBO);
	bind();
}

FrameBuffer::~FrameBuffer() {
	if (mTextureID)
		glDeleteTextures(1, &mTextureID);
	if (mRB0)
		glDeleteRenderbuffers(1, &mRB0);

	glDeleteFramebuffers(1, &mFBO);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glViewport(0, 0, mWidth, mHeight);
}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

FrameBuffer& FrameBuffer::withTexture() {
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withTextureMultisampled(const int multisampledCount) {
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mTextureID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampledCount, GL_RGB, mWidth, mHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mTextureID, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withTextureDepth() {
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTextureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withTextureDepthStencil() {
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mWidth, mHeight, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTextureID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withTextureDepthStencilMultisampled(const int multisampledCount) {
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mTextureID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampledCount, GL_RGB, mWidth, mHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mTextureID, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withRenderBufferDepthStencil() {
	glGenRenderbuffers(1, &mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, mRB0);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return *this;
}

FrameBuffer& FrameBuffer::withRenderBufferDepthStencilMultisampled(const int multisampledCount) {
	glGenRenderbuffers(1, &mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, mRB0);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisampledCount, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRB0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return *this;
}

void FrameBuffer::checkStatus() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("ERROR::FRAMEBUFFER::NOT_COMPLETE!\n");
	}
}
