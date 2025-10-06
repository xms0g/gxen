#pragma once

#include <SDL.h>
#include "glm/glm.hpp"

namespace GuiBackend {
void init(SDL_Window* window, SDL_GLContext context, const char* glsl_version);

void shutdown();

void newFrame();

void renderFrame();
}

namespace Ui {
bool beginEntity(const char* label);

void endEntity();

void colorField3(const char* label, glm::vec4& value, float speed = 0.01f, float sameLineOffset = 100.0f);
}
