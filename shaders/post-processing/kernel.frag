#version 410 core
in VS_OUT
{
    vec2 TexCoord;
} fs_in;

uniform sampler2D screenTexture;
uniform int type;

const float offset = 1.0 / 500.0;
const float blur[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
);
const float edge[9] = float[](
    1, 1, 1,
    1,-8, 1,
    1, 1, 1
);
const float sharpen[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);
const vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right
);

out vec4 fragColor;

void main() {
    float kernel[9];
    if (type == 1) kernel = blur;
    else if (type == 2) kernel = sharpen;
    else if (type == 3) kernel = edge;

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, fs_in.TexCoord.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    fragColor = vec4(col, 1.0);
}
