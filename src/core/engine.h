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

    bool isRunning;
    std::unique_ptr<Window> window;
    std::unique_ptr<Gui> gui;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Input> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Model> model;

    float deltaTime{};
    uint32_t millisecsPreviousFrame{0};

};