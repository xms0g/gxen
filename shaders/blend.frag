#version 410 core
in vec2 vTexcoord;

#include "material.glsl"

out vec4 fragColor;

void main() {
    fragColor = texture(material.texture_diffuse1, vTexcoord);
}