#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform MatrixBlock
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform mat3 normalMatrix;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vTexCoord = aTexCoord;
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = normalMatrix * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}