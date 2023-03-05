#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"
#include "Window.hpp"
#include "Shader.h"
#include "Model.h"
#include "Input.h"
#include "Camera.h"


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
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Input> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Model> model;


    float deltaTime{};
    uint32_t millisecsPreviousFrame{0};

};