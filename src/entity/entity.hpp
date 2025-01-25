#ifndef XNGN_ENTITY_HPP
#define XNGN_ENTITY_HPP

class Shader;
class Entity {
public:
    virtual ~Entity() = default;
    virtual Shader& getShader() const = 0;
    virtual void draw() const = 0;
};

#endif //XNGN_ENTITY_HPP
