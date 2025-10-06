#pragma once

#include <memory>
#include "glm/glm.hpp"
#include "renderSystem.h"
#include "../buffers/frameBuffer.h"
#include "../buffers/uniformBuffer.h"

class LightSystem;
class Window;
class Camera;
class guiSystem;
class Shader;

class ForwardRenderer final : public RenderSystem {
public:
	using RenderSystem::opaquePass;
	explicit ForwardRenderer();

	[[nodiscard]] GLuint getSceneTexture() const { return mSceneBuffer->texture(); }
	[[nodiscard]] GLuint getIntermediateTexture() const { return mIntermediateBuffer->texture(); }
	[[nodiscard]] uint32_t getSceneWidth() const { return mSceneBuffer->width(); }
	[[nodiscard]] uint32_t getSceneHeight() const { return mSceneBuffer->height(); }

	[[nodiscard]] const UniformBuffer& getCameraUBO() const { return *mCameraUBO; }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void configure(const Camera& camera);

	void updateBuffers(const Camera& camera) const;

	void batchEntities(const Camera& camera);

	void opaquePass();

	void transparentPass();

	void instancedPass();

	void transparentInstancedPass(const Camera& camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	void batchEntities(const Entity& entity, const Camera& camera);

	void updateCameraUBO(const Camera& camera) const;

	void updateLightUBO() const;

	void prepareInstanceData(const Entity& entity, const std::vector<glm::vec3>& positions, size_t instanceSize, uint32_t flags);

	struct {
		GLuint buffer;
		int offset{0};
		size_t size{0};
	} mStaticInstanceVBO;

	struct {
		GLuint buffer;
		int offset{0};
		size_t size{0};
	} mDynamicInstanceVBO;

	LightSystem* mLightSystem{};
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<FrameBuffer> mIntermediateBuffer;
	std::unique_ptr<UniformBuffer> mCameraUBO;
	std::unique_ptr<UniformBuffer> mLightUBO;

	using TransEntityBucket = std::vector<std::pair<float, Entity> >;
	TransEntityBucket mTransparentEntities;
	std::vector<Entity> mTransparentInstancedEntities;
	std::vector<Entity> mOpaqueEntities;
	std::vector<Entity> mInstancedEntities;
};