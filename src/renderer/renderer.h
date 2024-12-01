#pragma once

#include "glm/glm.hpp"

class Model;
class Renderer {
public:
    Renderer();

    void update(Model& model, glm::mat4 viewMatrix, float zoom);

    void render(Model& model);
};
