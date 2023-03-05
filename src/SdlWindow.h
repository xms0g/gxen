#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "Window.hpp"

class SDLWindow : public IWindow {
public:
    explicit SDLWindow(const char* title, bool fullscreen = false);

    ~SDLWindow() override;

    void updateFpsCounter(float dt) override;

    void clear() override;

    void swapBuffer() override;

private:
    const std::string m_title;
    SDL_Window* window;
    SDL_GLContext context;

    double previousSeconds{};
    double currentSeconds{};
    int frameCount{};

};
