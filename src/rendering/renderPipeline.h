#pragma once
#include <memory>
#include "../ECS/system.hpp"

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

	void configure(const Camera& camera) const;

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
	std::unique_ptr<DebugRenderer> mDebugRenderer;
	std::unique_ptr<ForwardRenderer> mForwardRenderer;
	std::unique_ptr<DeferredRenderer> mDeferredRenderer;
	std::unique_ptr<PostProcess> mPostProcess;

	// Frame Buffers
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<FrameBuffer> mIntermediateBuffer;
	std::unique_ptr<FrameBuffer> mGBuffer;
	// Uniform Buffers
	std::unique_ptr<UniformBuffer> mCameraUBO;
	// Shaders
	std::unique_ptr<Shader> mGShader;
	std::unique_ptr<Shader> mDeferredLigthingShader;

	// Render queue
	struct {
		std::vector<InstanceGroup> opaqueInstancedGroups;
		std::vector<InstanceGroup> blendInstancedGroups;
		std::vector<RenderGroup> debugGroups;
		std::vector<RenderGroup> shadowCasterGroups;
		std::vector<RenderGroup> forwardOpaqueGroups;
		std::vector<RenderGroup> deferredGroups;
		std::vector<RenderGroup> blendGroups;
	} renderQueues;
};
