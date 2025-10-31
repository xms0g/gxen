#pragma once
#include <memory>

class Camera;
class Registry;
class ForwardRenderer;
class DeferredRenderer;
class DebugRenderer;
class GuiSystem;
class LightSystem;
class SkyboxSystem;
class ShadowSystem;
class PostProcess;

class RenderPipeline {
public:
	explicit RenderPipeline(Registry* registry);

	~RenderPipeline();

	PostProcess& postProcess() const;

	void configure(const Camera& camera) const;

	void render(const Camera& camera) const;

private:
	ForwardRenderer* mForwardRenderer{};
	DeferredRenderer* mDeferredRenderer{};
	DebugRenderer* mDebugRenderer{};
	LightSystem* mLightSystem{};
	SkyboxSystem* mSkyboxSystem{};
	ShadowSystem* mShadowSystem{};

	std::unique_ptr<PostProcess> mPostProcess;
};
