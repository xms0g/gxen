#include "light.h"
#include "../renderer/shader.h"
#include "glm/ext/matrix_transform.hpp"

Light::Light(const glm::vec3 pos, const std::string& vertexPath, const std::string& fragmentPath) {
    position = pos;
    mShader = std::make_unique<Shader>(vertexPath, fragmentPath);

    constexpr float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &mVAO);

    glGenBuffers(1, &mVBO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(mVAO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
}

void Light::draw() const {
    mShader->activate();

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
