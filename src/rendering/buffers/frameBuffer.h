#pragma once
#include <cstdint>

class FrameBuffer {
public:
	FrameBuffer(int width, int height);

	~FrameBuffer();

	[[nodiscard]] int width() const { return mWidth; }
	[[nodiscard]] int height() const { return mHeight; }
	[[nodiscard]] uint32_t texture() const { return mTextureID; }

	void bind() const;

	void bindForRead() const;

	void bindForDraw() const;

	void unbind();

	FrameBuffer& withTexture();

	FrameBuffer& withTextureMultisampled(int multisampledCount);

	FrameBuffer& withTextureDepth();

	FrameBuffer& withTextureCubeMapDepth();

	FrameBuffer& withTextureDepthStencil();

	FrameBuffer& withTextureDepthStencilMultisampled(int multisampledCount);

	FrameBuffer& withRenderBufferDepthStencil();

	FrameBuffer& withRenderBufferDepthStencilMultisampled(int multisampledCount);

	void checkStatus();

private:
	int mWidth{0};
	int mHeight{0};
	uint32_t mFBO{0};
	uint32_t mRBO{0};
	uint32_t mTextureID{0};
};
