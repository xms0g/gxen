#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D hdrTexture;
uniform float exposure;

out vec4 fragColor;

void main() {
    vec3 hdrColor = texture(hdrTexture, fs_in.TexCoord).rgb;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    fragColor = vec4(result, 1.0);
}