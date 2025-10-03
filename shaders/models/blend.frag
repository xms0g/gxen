#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

#include "material.glsl"

out vec4 fragColor;

void main() {
    fragColor = texture(material.texture_diffuse1, fs_in.TexCoord);
}