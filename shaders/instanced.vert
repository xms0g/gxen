#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 7) in mat4 aInstanceMatrix;
layout (location = 11) in mat3 aInstanceNormalMatrix;

#include "camera.glsl"

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} vs_out;

void main() {
    vs_out.TexCoord = aTexCoord;
    vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = aInstanceNormalMatrix * aNormal;

    gl_Position = projection * view  * worldPos;
}