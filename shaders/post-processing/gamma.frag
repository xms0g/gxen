#version 410 core
in vec2 vTexCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec3 color = texture(screenTexture, vTexCoord).rgb;
    vec3 mapped = pow(color, vec3(1.0 / 2.2));
    fragColor = vec4(mapped, 1.0);
}
