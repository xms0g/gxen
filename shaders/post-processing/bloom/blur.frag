#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D screenTexture;
uniform bool horizontal;

const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

out vec4 fragColor;

void main() {
    vec2 tex_offset = 1.0 / textureSize(screenTexture, 0); // gets size of single texel
    vec3 result = texture(screenTexture, fs_in.TexCoord).rgb * weight[0];

    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(screenTexture, fs_in.TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(screenTexture, fs_in.TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else {
        for(int i = 1; i < 5; ++i) {
            result += texture(screenTexture, fs_in.TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(screenTexture, fs_in.TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    fragColor = vec4(result, 1.0);
}