#pragma once
#include <memory>

class ShadowSystem;
class DebugRenderer;
class SkyboxSystem;
class LightSystem;
class ForwardRenderer;
class GuiSystem;
class PostProcess;
class Window;
class Registry;
class Input;
class Camera;

class Engine {
public:
	Engine();

	~Engine();

	void init(Registry* registry);

	void configure() const;

	void run();

private:
	void preProcess() const;

	void render() const;

	void postProcess() const;

	float mDeltaTime{};
	uint32_t mMillisecsPreviousFrame{0};
	// Engine
	bool isRunning{true};

	Registry* mRegistry{};
	ForwardRenderer* mForwardRenderer{};
	DebugRenderer* mDebugRenderer{};
	GuiSystem* mGuiSystem{};
	LightSystem* mLightSystem{};
	SkyboxSystem* mSkyboxSystem{};
	ShadowSystem* mShadowSystem{};

	std::unique_ptr<PostProcess> mPostProcess;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;
	std::unique_ptr<Input> mInput;
};
