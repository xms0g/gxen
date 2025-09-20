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
    float mDeltaTime{};
    uint32_t mMillisecsPreviousFrame{0};
    // Engine
    bool isRunning{true};

    Scene* mScene{};
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Renderer> mRenderer;
};