#version 410 core
in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} fs_in;

#include "phong.glsl"
#include "cameraUBO.glsl"

out vec4 fragColor;

void main() {
    vec3 texColor = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    bool hasLights = numDirLights + numPointLights + numSpotLights > 0;
    vec3 result = mix(texColor, calculateLights(fs_in.Normal, fs_in.FragPos, viewPos), float(hasLights));

    fragColor = vec4(result, 1.0);
}