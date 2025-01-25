#ifndef XNGN_LIGHT_H
#define XNGN_LIGHT_H

#include <memory>
#include "glm/glm.hpp"
#include "../entity/entity.hpp"

class Shader;
class Light : public Entity {
public:
    Light(glm::vec3 pos, const std::string& vertexPath, const std::string& fragmentPath);

    [[nodiscard]] glm::vec3 pos() const { return mPos; }

    void draw() const override;

    [[nodiscard]] Shader& getShader() const override { return *mShader; }

private:
    std::unique_ptr<Shader> mShader;
    glm::vec3 mPos;
};

#endif //XNGN_LIGHT_H
