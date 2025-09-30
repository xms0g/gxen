#pragma once

#include <memory>

class LightSystem;
class RenderSystem;
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

    void run();

private:
    float mDeltaTime{};
    uint32_t mMillisecsPreviousFrame{0};
    // Engine
    bool isRunning{true};

    Registry* mRegistry{};
    RenderSystem* mRenderSystem;
	GuiSystem* mGuiSystem;
	LightSystem* mLightSystem;

	std::unique_ptr<PostProcess> mPostProcess;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
};