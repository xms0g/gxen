#include "light.h"
#include "../renderer/shader.h"

Light::Light(glm::vec3 pos, const std::string& vertexPath, const std::string& fragmentPath): mPos(pos) {
    mShader = std::make_unique<Shader>(vertexPath, fragmentPath);
}

void Light::draw() const {

}
