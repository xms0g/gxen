#pragma once

class Camera;
class SDL_Window;
class Input {
public:
    Input() = default;

    void Process(Camera& camera, SDL_Window* window, float dt, bool& isRunning);

private:
    void ProcessKeyboard(Camera& camera, float dt, bool& isRunning);

    void CalculateMouseOffset();

    int mouseX{}, mouseY{};

};
