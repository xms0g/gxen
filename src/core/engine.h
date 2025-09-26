#pragma once

#include <memory>

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
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
};