#pragma once
#include <memory>

class ShadowManager;
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

class XEngine {
public:
    XEngine();

    ~XEngine();

    void init(Registry* registry);

	void configure() const;

    void run();

private:
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

	std::unique_ptr<PostProcess> mPostProcess;
	std::unique_ptr<ShadowManager> mShadowManager;

	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
};