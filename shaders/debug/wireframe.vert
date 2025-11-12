#version 410 core
layout (location = 0) in vec3 aPos;

#include "ub/camera.glsl"

uniform mat4 model;

void main() {
    gl_Position = view * model * vec4(aPos, 1.0);
}