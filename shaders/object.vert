#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

#include "camera.glsl"

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 persLightSpaceMatrix;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec4 FragPosPersLightSpace;
} vs_out;

void main() {
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.FragPosPersLightSpace = persLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = projection * view  * model * vec4(aPos, 1.0);
}