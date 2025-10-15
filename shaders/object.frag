#version 410 core
in VS_OUT
{
    vec2 TexCoord;
    mat3 TBN;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec4 FragPosPersLightSpace;
} fs_in;

#include "blinnPhong.glsl"
#include "camera.glsl"

out vec4 fragColor;

void main() {
    vec3 N;
    if (material.hasNormalMap) {
        N = texture(material.texture_normal1, fs_in.TexCoord).rgb;
        // transform normal vector to range [-1,1]
        N = N * 2.0 - 1.0;
        N = normalize(fs_in.TBN * N);
    } else {
        N = normalize(fs_in.TBN[2]);
    }
    vec3 texColor = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    bool hasLights = lightCount.x > 0 || lightCount.y > 0 || lightCount.z > 0;
    vec3 result = mix(texColor, calculateLights(N, fs_in.FragPos, viewPos), float(hasLights));

    fragColor = vec4(result, 1.0);
}