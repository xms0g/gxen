#version 410 core
in vec2 vTexCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec4 color = texture(screenTexture, vTexCoord);

    float r = dot(color, vec4(0.393f, 0.769f, 0.189f, 0.0f));
    float g = dot(color, vec4(0.349f, 0.686f, 0.168f, 0.0f));
    float b = dot(color, vec4(0.272f, 0.534f, 0.131f, 0.0f));

    fragColor = vec4(r, g, b, 1.0);
}