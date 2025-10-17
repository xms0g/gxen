#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

#include "camera.glsl"
#include "shadow.glsl"

uniform mat4 model;
uniform mat3 normalMatrix;

out VS_OUT
{
    vec2 TexCoord;
    mat3 TBN;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec4 FragPosPersLightSpace;
} vs_out;

void main() {
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.FragPosPersLightSpace = persLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = projection * view  * model * vec4(aPos, 1.0);
}