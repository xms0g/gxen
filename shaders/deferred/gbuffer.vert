#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

#include "camera.glsl"

uniform mat4 model;
uniform mat3 normalMatrix;

out VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    mat3 TBN;
    vec3 TangentViewDir;
} vs_out;

void main() {
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoord = aTexCoord;

    vec3 viewDir = normalize(viewPos.xyz - vs_out.FragPos);
    vs_out.TangentViewDir = normalize(transpose(vs_out.TBN) * viewDir);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
