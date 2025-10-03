#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;


uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec3 gammaCorrected = pow(texture(screenTexture, fs_in.TexCoord).rgb, vec3(1.0 / 2.2));
    fragColor = vec4(gammaCorrected, 1.0);
}
