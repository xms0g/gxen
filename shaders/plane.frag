#version 410 core

in vec2 vTexcoord;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

out vec4 fragColor;

void main() {
    fragColor = texture(material.texture_diffuse1, vTexcoord);
}