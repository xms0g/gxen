#version 410 core

in vec2 vTexcoord;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform bool useTexture;
uniform Material material;
uniform vec3 color;

out vec4 fragColor;

void main() {
    vec3 color = useTexture ? texture(material.texture_diffuse1, vTexcoord).rgb : color;
    fragColor = vec4(color, 1.0);
}