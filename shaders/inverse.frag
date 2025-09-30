#version 410 core
in vec2 vTexCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec3 color = vec3(1.0 - texture(screenTexture, vTexCoord));
    fragColor = vec4(color, 1.0);
}
