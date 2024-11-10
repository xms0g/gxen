#pragma once

#include <memory>

class Shader;
class Model;
class Camera;
class Window;
class Input;
class Gui;
class Engine {
public:
    Engine();

    ~Engine();

    void init();

    void run();

private:
    void ProcessInput();

    void Update();

    void Render();

    bool isRunning{true};
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
    std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Shader> mShader;
    std::unique_ptr<Model> mModel;

    float deltaTime{};
    uint32_t millisecsPreviousFrame{0};

};