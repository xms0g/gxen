#pragma once

#include <SDL.h>

#include "glm/vec3.hpp"

class Gui {
public:
    Gui(SDL_Window* window, SDL_GLContext gl_context);

    ~Gui();

    void render();

    void updateFpsCounter(float dt);

	void updateTransform(glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) const;

private:
    void renderGraphicsInfo() const;
	void renderTransform();

	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;
    // Frame
    double mPreviousSeconds{0.0};
    double mCurrentSeconds{0.0};
    uint32_t mFPS{0};
    uint32_t mCurrentFrameCount{0};
};
