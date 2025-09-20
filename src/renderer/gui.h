#pragma once

#include <SDL.h>

class Gui {
public:
    Gui(SDL_Window* window, SDL_GLContext gl_context);

    ~Gui();

    void render();

    void updateFpsCounter(float dt);

private:
    void renderGraphicsInfo() const;

    // Frame
    double mPreviousSeconds{0.0};
    double mCurrentSeconds{0.0};
    uint32_t mFPS{0};
    uint32_t mCurrentFrameCount{0};
};
