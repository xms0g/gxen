#ifndef XNGN_ENTITY_HPP
#define XNGN_ENTITY_HPP

#include "glm/glm.hpp"

class Shader;
struct Entity {
    float rotation{0.0f};
    float scale{1.0f};
    glm::vec3 position{0.0f};

    virtual ~Entity() = default;
    [[nodiscard]] virtual Shader* getShader() const = 0;
    virtual void draw() const = 0;
};

#endif //XNGN_ENTITY_HPP
