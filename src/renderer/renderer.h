#pragma once

#include "glm/glm.hpp"

class Model;
class Renderer {
public:
    Renderer();

    void update(Model& model, glm::mat4 viewMatrix, float zoom, float rt);

    void render(Model& model);
};
