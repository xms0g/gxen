#pragma once

#include <memory>

class Scene;
class Input;
class Renderer;

class XEngine {
public:
    XEngine();

    ~XEngine();

    void init(Scene* scene);

    void run();

private:
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

    Scene* mScene;
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Renderer> mRenderer;
};