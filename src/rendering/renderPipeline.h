#pragma once
#include <memory>
#include <unordered_map>
#include "../ECS/system.hpp"

class Shader;
class FrameBuffer;
class UniformBuffer;
class Camera;
class Registry;
class ForwardRenderer;
class DebugRenderer;
class GuiSystem;
class LightSystem;
class SkyboxSystem;
class ShadowManager;
class PostProcess;

class RenderPipeline final : public System {
public:
	explicit RenderPipeline(Registry* registry);

	~RenderPipeline() override;

	[[nodiscard]] PostProcess& postProcess() const;

	void configure(const Camera& camera) const;

	void batchEntities(const Camera& camera);

	void render(const Camera& camera);

private:
	void updateBuffers(const Camera& camera) const;

	void frustumCullingPass(const Camera& camera) const;

	void beginSceneRender() const;

	void endSceneRender() const;

	void batchEntities(const Entity& entity, const Camera& camera);
	// Systems
	//DeferredRenderer* mDeferredRenderer{};
	LightSystem* mLightSystem{};
	SkyboxSystem* mSkyboxSystem{};

	std::unique_ptr<ShadowManager> mShadowManager;
	std::unique_ptr<DebugRenderer> mDebugRenderer;
	std::unique_ptr<ForwardRenderer> mForwardRenderer;
	std::unique_ptr<PostProcess> mPostProcess;

	// Frame Buffers
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<FrameBuffer> mHDRBuffer;
	std::unique_ptr<FrameBuffer> mIntermediateBuffer;

	// Uniform Buffers
	std::unique_ptr<UniformBuffer> mCameraUBO;

	// Render queue
	struct {
		using TransEntityBucket = std::vector<std::pair<float, Entity> >;
		TransEntityBucket transparentEntities;
		std::vector<Entity> transparentInstancedEntities;
		std::vector<Entity> opaqueInstancedEntities;
		std::vector<Entity> debugEntities;
		std::unordered_map<Shader*, std::vector<Entity>> opaqueBatches;
	} renderQueue;
};
