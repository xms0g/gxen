#version 410 core
in vec2 vTexcoord;

#include "material.glsl"

uniform bool useTexture;
uniform vec3 color;

out vec4 fragColor;

void main() {
    vec3 color = useTexture ? texture(material.texture_diffuse1, vTexcoord).rgb : color;
    fragColor = vec4(color, 1.0);
}