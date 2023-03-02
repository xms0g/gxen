#version 400 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outFragPos;
out vec3 outNormal;
out vec2 outTexcoord;

void main() {
    outFragPos = vec3(model * vec4(inPos, 1.0));
    outNormal = mat3(transpose(inverse(model))) * inNormal;;
    outTexcoord = inTexcoord;

    gl_Position = projection * view  * vec4(outFragPos, 1.0);
}