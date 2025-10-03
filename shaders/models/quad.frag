#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(screenTexture, fs_in.TexCoord);
}
