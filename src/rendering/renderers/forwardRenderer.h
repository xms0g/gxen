#pragma once

#include <memory>
#include "../buffers/frameBuffer.h"
#include "renderSystem.h"
#include "../buffers/uniformBuffer.h"

class LightSystem;
class Window;
class Camera;
class guiSystem;
class Shader;

class ForwardRenderer final : public RenderSystem {
public:
	explicit ForwardRenderer();

	[[nodiscard]] GLuint getSceneTexture() const { return mSceneBuffer->texture(); }
	[[nodiscard]] uint32_t getSceneWidth() const { return mSceneBuffer->width(); }
	[[nodiscard]] uint32_t getSceneHeight() const { return mSceneBuffer->height(); }

	const UniformBuffer& getCameraUBO() const { return *mCameraUBO; }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void configure(const Camera& camera) const;

	void render(const Camera& camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	void updateCameraUBO(const Camera& camera) const;

	void updateLightUBO() const;

	LightSystem* mLightSystem{};
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<UniformBuffer> mCameraUBO;
	std::unique_ptr<UniformBuffer> mLightUBO;
};