#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "renderSystem.h"

struct ShadowData;
class FrameBuffer;
class UniformBuffer;
class LightSystem;
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

	const std::vector<Entity>& getOpaqueEntities() { return mOpaqueEntities; }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void configure(const Camera& camera);

	void updateBuffers(const Camera& camera) const;

	void batchEntities(const Camera& camera);

	void opaquePass(const ShadowData& shadowData);

	void transparentPass();

	void instancedPass(const ShadowData& shadowData);

	void transparentInstancedPass(const Camera& camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	void batchEntities(const Entity& entity, const Camera& camera);

	void updateCameraUBO(const Camera& camera) const;

	void updateLightUBO() const;

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
