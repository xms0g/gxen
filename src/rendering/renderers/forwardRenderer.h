#pragma once
#include <memory>
#include <unordered_map>
#include "glm/glm.hpp"
#include "renderSystem.h"

class FrameBuffer;
class UniformBuffer;
class Window;
class Camera;
class guiSystem;
class Shader;

class ForwardRenderer final : public RenderSystem {
public:
	using RenderSystem::opaquePass;

	explicit ForwardRenderer();

	~ForwardRenderer() override;

	[[nodiscard]] uint32_t getSceneTexture() const;

	[[nodiscard]] uint32_t getIntermediateTexture() const;

	[[nodiscard]] uint32_t getSceneWidth() const;

	[[nodiscard]] uint32_t getSceneHeight() const;

	[[nodiscard]] const UniformBuffer& getCameraUBO() const { return *mCameraUBO; }

	void configure(const Camera& camera, const UniformBuffer& lightUBO);

	void updateBuffers(const Camera& camera) const;

	void batchEntities(const Camera& camera);

	void opaquePass(const std::array<uint32_t, 3>& shadowMaps);

	void transparentPass();

	void instancedPass();

	void transparentInstancedPass(const Camera& camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	void batchEntities(const Entity& entity, const Camera& camera);

	void updateCameraUBO(const Camera& camera) const;

	void prepareInstanceData(const Entity& entity,
	                         const std::vector<glm::vec3>& positions,
	                         size_t instanceSize,
	                         uint32_t flags);

	struct {
		uint32_t buffer;
		int offset{0};
	} mStaticInstanceVBO;

	struct {
		uint32_t buffer;
		int offset{0};
	} mDynamicInstanceVBO;

	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<FrameBuffer> mHDRBuffer;
	std::unique_ptr<FrameBuffer> mIntermediateBuffer;

	std::unique_ptr<UniformBuffer> mCameraUBO;

	using TransEntityBucket = std::vector<std::pair<float, Entity> >;
	TransEntityBucket mTransparentEntities;
	std::vector<Entity> mTransparentInstancedEntities;
	std::vector<Entity> mInstancedEntities;
	std::unordered_map<Shader*, std::vector<Entity>> mOpaqueBatches;
};
