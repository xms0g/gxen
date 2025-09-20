#pragma once

#include <memory>

class Entity;
class Renderer {
public:
    Renderer();

    void render(std::unique_ptr<Entity>& entity);
};
