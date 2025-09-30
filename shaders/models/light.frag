#version 410 core
#include "material.glsl"

uniform bool useTexture;

out vec4 fragColor;

void main() {
    fragColor = vec4(material.color, 1.0);
}
