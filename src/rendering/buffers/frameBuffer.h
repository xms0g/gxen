#pragma once
#include <cstdint>
#include <vector>

class FrameBuffer {
public:
	FrameBuffer(int width, int height);

	~FrameBuffer();

	[[nodiscard]] int width() const { return mWidth; }
	[[nodiscard]] int height() const { return mHeight; }
	[[nodiscard]] uint32_t texture() const { return mTextureIDs.front(); }
	[[nodiscard]] std::vector<uint32_t>& textures() { return mTextureIDs; }

	void bind() const;

	void bindForRead() const;

	void bindForDraw() const;

	void unbind();

	FrameBuffer& withTexture();

	FrameBuffer& withTextureMultisampled(int multisampledCount);

	FrameBuffer& withTexture16F();

	FrameBuffer& withTextureDepth();

	FrameBuffer& withTextureCubemapDepth();

	FrameBuffer& withTextureDepthStencil();

	FrameBuffer& withTextureDepthStencilMultisampled(int multisampledCount);

	FrameBuffer& withRenderBufferDepth(uint32_t depthFormat);

	FrameBuffer& withRenderBufferDepthMultisampled(int multisampledCount, uint32_t depthFormat);

	FrameBuffer& withRenderBufferDepthStencil();

	FrameBuffer& withRenderBufferDepthStencilMultisampled(int multisampledCount);

	FrameBuffer& configureAttachments();

	void checkStatus();

private:
	int mWidth{0};
	int mHeight{0};
	uint32_t mFBO{0};
	uint32_t mRBO{0};
	std::vector<uint32_t> mTextureIDs;
	std::vector<uint32_t> mAttachments;
};
