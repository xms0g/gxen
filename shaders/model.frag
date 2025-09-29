#version 410 core
in vec2 vTexcoord;
in vec3 vNormal;
in vec3 vFragPos;

#include "phong.glsl"

uniform vec3 viewPos;

out vec4 fragColor;

void main() {
    vec3 result = calculateLights(vNormal, vFragPos, viewPos);

    vec3 texColor = texture(material.texture_diffuse1, vTexcoord).rgb;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    float hasLights = float(numDirLights + numPointLights + numSpotLights > 0);
    result = mix(texColor, result, hasLights);

    fragColor = vec4(pow(result, vec3(1.0 / 2.2)), 1.0);
}