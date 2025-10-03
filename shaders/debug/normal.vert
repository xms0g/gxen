#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

#include "camera.glsl"

uniform mat4 model;
uniform mat3 normalMatrix;

out VS_OUT
{
    vec3 Normal;
} vs_out;

void main() {
    vs_out.Normal = normalMatrix * aNormal;
    gl_Position = view  * model * vec4(aPos, 1.0);
}