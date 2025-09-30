#version 410 core
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vFragPos;

#include "phong.glsl"

uniform bool useTexture;
uniform vec3 viewPos;

out vec4 fragColor;

void main() {
    vec3 texColor = useTexture ? texture(material.texture_diffuse1, vTexCoord).rgb : material.color;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    bool hasLights = numDirLights + numPointLights + numSpotLights > 0;
    vec3 result = mix(texColor, calculateLights(vNormal, vFragPos, viewPos), float(hasLights));

    fragColor = vec4(result, 1.0);
}