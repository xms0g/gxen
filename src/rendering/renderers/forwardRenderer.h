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

	[[nodiscard]] const UniformBuffer& getCameraUBO() const { return *mCameraUBO; }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void configure(const Camera& camera) const;

	void render(const Camera& camera);

	void instancedRender();

	void transparentPass();

	void transparentInstancedPass(const Camera& camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	void batchEntities(const Entity& entity, const Camera& camera);

	void updateCameraUBO(const Camera& camera) const;

	void updateLightUBO() const;

	void prepareInstanceData(const Entity& entity, uint32_t flags) const;

	GLuint mStaticInstanceVBO, mDynamicInstanceVBO;
	LightSystem* mLightSystem{};
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<UniformBuffer> mCameraUBO;
	std::unique_ptr<UniformBuffer> mLightUBO;

	using TransEntityBucket = std::vector<std::pair<float, Entity> >;
	TransEntityBucket mTransparentEntities;
	std::vector<Entity> mTransparentInstancedEntities;
	std::vector<Entity> mOpaqueEntities;
	std::vector<Entity> mInstancedEntities;
};