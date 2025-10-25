#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

out vec4 fragColor;

void main() {
    vec3 hdrColor = texture(screenTexture, fs_in.TexCoord).rgb;
    vec3 bloomColor = texture(bloomBlur, fs_in.TexCoord).rgb;
    hdrColor += bloomColor; // additive blending

    fragColor = vec4(hdrColor, 1.0);
}
