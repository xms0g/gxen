#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec2 vTexcoord;
out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vTexcoord = aTexcoord;
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = normalMatrix * aNormal;

    gl_Position = projection * view  * model * vec4(aPos, 1.0);
}