#pragma once

#include <SDL.h>

class Gui {
public:
    Gui(SDL_Window* window, SDL_GLContext gl_context);

    ~Gui();

    void setFPS(double fps) { mFPS = fps; }

    void render();

private:
    void renderGraphicsInfo() const;

    double mFPS{0.0};
    uint64_t mGenerationCount{0};
    uint32_t mAliveCellCount{0};
};
