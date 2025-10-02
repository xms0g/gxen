#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

#include "cameraUBO.glsl"

uniform mat4 model;
uniform mat3 normalMatrix;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} vs_out;

void main() {
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalMatrix * aNormal;

    gl_Position = projection * view  * model * vec4(aPos, 1.0);
}