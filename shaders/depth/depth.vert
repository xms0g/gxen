#version 410 core
layout (location = 0) in vec3 aPos;

#include "ub/camera.glsl"

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}