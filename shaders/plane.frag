#version 410 core
in vec2 vTexcoord;
in vec3 vNormal;
in vec3 vFragPos;

#include "phong.glsl"

uniform bool useTexture;
uniform vec3 viewPos;

out vec4 fragColor;

void main() {
    vec3 result;
    float hasLights = float(numDirLights + numPointLights + numSpotLights > 0);

    if (hasLights > 0)
        result = calculateLights(vNormal, vFragPos, viewPos);
    else {
        result = useTexture ? texture(material.texture_diffuse1, vTexcoord).rgb : material.color;
    }

    fragColor = vec4(result, 1.0);
}