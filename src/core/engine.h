#pragma once

#include <memory>

class Scene;
class Camera;
class Window;
class Input;
class Gui;
class Renderer;

class XEngine {
public:
    XEngine();

    ~XEngine();

    void init();

    void run();

    void addScene(std::unique_ptr<Scene>& scene);

private:
    void processInput();

    void update();

    void render();

    void updateFpsCounter();

    // Frame
    double mPreviousSeconds{0.0};
    double mCurrentSeconds{0.0};
    float mDeltaTime{};
    uint32_t mFPS{0};
    uint32_t mCurrentFrameCount{0};
    uint32_t mMillisecsPreviousFrame{0};

    // Engine
    bool isRunning{true};
    float rotationAngle; //TODO: will be removed

    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
    std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<Scene> mScene;

};