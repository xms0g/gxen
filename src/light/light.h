#ifndef XNGN_LIGHT_H
#define XNGN_LIGHT_H

#include <memory>
#include "glm/glm.hpp"
#include "../entity/entity.hpp"

class Shader;
class Light final : public Entity {
public:
    Light(glm::vec3 pos, const std::string& vertexPath, const std::string& fragmentPath);

    [[nodiscard]] glm::vec3 pos() const { return position; }

    void draw() const override;

    [[nodiscard]] Shader* getShader() const override { return mShader.get(); }

private:
    unsigned int mVAO{};
    unsigned int mVBO{};
    std::unique_ptr<Shader> mShader;
};

#endif //XNGN_LIGHT_H
