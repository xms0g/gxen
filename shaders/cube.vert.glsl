#version 400 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexcoord;
layout (location = 2) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 outTexcoord;
out vec3 outNormal;
out vec3 fragPos;

void main() {
    gl_Position = projection * view * model * vec4(inPos, 1.0);
    outTexcoord = vec2(inTexcoord.x, inTexcoord.y);
    outNormal = inNormal;
    fragPos = vec3(model * vec4(inPos, 1.0));
}