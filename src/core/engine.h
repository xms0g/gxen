#pragma once

#include <memory>

class Shader;
class Model;
class Camera;
class Window;
class Input;
class Gui;
class XEngine {
public:
    XEngine();

    ~XEngine();

    void init();

    void run();

private:
    void ProcessInput();

    void Update();

    void Render();

    void updateFpsCounter();

    // Frame
    double mPreviousSeconds{0.0};
    double mCurrentSeconds{0.0};
    uint32_t mCurrentFrameCount{0};
    uint32_t mFPS{0};

    // Engine
    bool isRunning{true};

    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
    std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Shader> mShader;
    std::unique_ptr<Model> mModel;

    float mDeltaTime{};
    uint32_t mMillisecsPreviousFrame{0};

};