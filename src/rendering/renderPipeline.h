#pragma once
#include <memory>
#include <vector>
#include "renderContext/renderQueue.hpp"
#include "../ECS/system.hpp"

class DeferredLightingPass;
class DeferredGeometryPass;
class IRenderPass;
struct InstanceGroup;
struct RenderGroup;
class Mesh;
class Shader;
class FrameBuffer;
class UniformBuffer;
class Camera;
class Registry;
class ForwardRenderer;
class DeferredRenderer;
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

	void configure(const Camera& camera);

	void batchEntities();

	void render(const Camera& camera);

private:
	void updateBuffers(const Camera& camera) const;

	void frustumCullingPass(const Camera& camera) const;

	void beginSceneRender() const;

	void endSceneRender() const;

	void batchEntity(const Entity& entity);

	void sortEntities(const Camera& camera);
	// Systems
	LightSystem* mLightSystem{};
	SkyboxSystem* mSkyboxSystem{};

	std::unique_ptr<ShadowManager> mShadowManager;
	std::unique_ptr<PostProcess> mPostProcess;
	// Frame Buffers
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<FrameBuffer> mIntermediateBuffer;
	// Uniform Buffers
	std::unique_ptr<UniformBuffer> mCameraUBO;
	// Render queue
	RenderQueue mRenderQueue;
	std::shared_ptr<DeferredGeometryPass> mDeferredGeometryPass;
	std::shared_ptr<DeferredLightingPass> mDeferredLightingPass;
	std::vector<std::shared_ptr<IRenderPass>> mRenderPasses;
};
