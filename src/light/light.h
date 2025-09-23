#ifndef XNGN_LIGHT_H
#define XNGN_LIGHT_H

#include <memory>
#include "glm/glm.hpp"

class Shader;
class Light {
public:
    Light(glm::vec3 pos, const std::string& vertexPath, const std::string& fragmentPath);

    void draw() const ;

    [[nodiscard]] Shader* shader() const  { return mShader.get(); }

private:
    unsigned int mVAO{};
    unsigned int mVBO{};
    std::unique_ptr<Shader> mShader;
};

#endif //XNGN_LIGHT_H
