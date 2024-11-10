#pragma once

#include <memory>
#include "window.hpp"
#include "shader.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "gui.h"

class Engine {
public:
    Engine();

    [[nodiscard]] bool IsRunning() const;

    void ProcessInput();

    void Update();

    void Render();

private:
    bool isRunning;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<Gui> gui;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Input> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Model> model;


    float deltaTime{};
    uint32_t millisecsPreviousFrame{0};

};