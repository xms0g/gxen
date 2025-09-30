#version 410 core
in vec2 vTexCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(screenTexture, vTexCoord);
}
