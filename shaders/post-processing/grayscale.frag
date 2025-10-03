#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;


uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    vec3 color = texture(screenTexture, fs_in.TexCoord).rgb;
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    fragColor = vec4(vec3(gray), 1.0);
}
