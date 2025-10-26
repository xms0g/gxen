#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec3 color = texture(screenTexture, fs_in.TexCoord).rgb;

    float r = dot(color, vec3(0.393f, 0.769f, 0.189f));
    float g = dot(color, vec3(0.349f, 0.686f, 0.168f));
    float b = dot(color, vec3(0.272f, 0.534f, 0.131f));

    fragColor = vec4(r, g, b, 1.0);
}