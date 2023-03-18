#include "Input.h"
#include <SDL2/SDL.h>
#include "Configs.hpp"
#include "Camera.h"
#include "../libs/imgui/imgui_impl_sdl.h"


Input::Input() :
        mouseLastX(SCR_WIDTH / 2.0f),
        mouseLastY(SCR_HEIGHT / 2.0f),
        firstMouse(true) {}


void Input::Process(Camera& camera, float dt, bool& isRunning) {
    SDL_Event event;
    SDL_PollEvent(&event);

    ImGui_ImplSDL2_ProcessEvent(&event);
    ImGuiIO& io = ImGui::GetIO();

    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
    io.MousePos = ImVec2(mouseX, mouseY);
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

    ProcessKeyboard(camera, dt, isRunning);
    ProcessMouse(camera);
}


void Input::ProcessKeyboard(Camera& camera, float dt, bool& isRunning) {
    auto* keystate = SDL_GetKeyboardState(nullptr);

    if (keystate[SDL_SCANCODE_ESCAPE]) {
        isRunning = false;
    } else if (keystate[SDL_SCANCODE_W]) {
        camera.ProcessKeyboard(FORWARD, dt);
    } else if (keystate[SDL_SCANCODE_S]) {
        camera.ProcessKeyboard(BACKWARD, dt);
    } else if (keystate[SDL_SCANCODE_A]) {
        camera.ProcessKeyboard(LEFT, dt);
    } else if (keystate[SDL_SCANCODE_D]) {
        camera.ProcessKeyboard(RIGHT, dt);
    }
}


void Input::ProcessMouse(Camera& camera) {
    SDL_GetMouseState(&mouseX, &mouseY);
    CalculateMouseOffset();
    //camera.ProcessMouseMovement(mouseOffsetX, mouseOffsetY);
}


void Input::CalculateMouseOffset() {
    float xpos = static_cast<float>(mouseX);
    float ypos = static_cast<float>(mouseY);

    if (firstMouse) {
        mouseLastX = xpos;
        mouseLastY = ypos;
        firstMouse = false;
    }

    mouseOffsetX = xpos - mouseLastX;
    mouseOffsetY = mouseLastY - ypos; // reversed since y-coordinates go from bottom to top

    mouseLastX = xpos;
    mouseLastY = ypos;
}
