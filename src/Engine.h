#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"


class Camera;
class Input;
class Shader;
class Model;
class Engine {
public:
    Engine();

    ~Engine();

    [[nodiscard]] bool IsRunning() const;

    void ProcessInput();

    void Update();

    void Render();

private:
    bool isRunning;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Input> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Model> model;

    SDL_Window* window;
    SDL_GLContext context;

    float deltaTime{};
    uint32_t millisecsPreviousFrame{0};

};