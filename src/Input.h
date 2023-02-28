#pragma once

class Camera;
class Input {
public:
    Input();

    void Process(Camera& camera, float dt, bool& isRunning);

private:
    void ProcessKeyboard(Camera& camera, float dt, bool& isRunning);

    void ProcessMouse(Camera& camera);

    void CalculateMouseOffset();

    int mouseX{}, mouseY{};
    float mouseLastX, mouseLastY;
    float mouseOffsetX{}, mouseOffsetY{};
    bool firstMouse;

};
