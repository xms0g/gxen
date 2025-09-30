#version 410 core
in vec2 vTexCoord;
#include "post-processing/sharpen.glsl"
#include "post-processing/blur.glsl"
#include "post-processing/edgeDetect.glsl"

uniform sampler2D screenTexture;
uniform int type;

out vec4 fragColor;

const float offset = 1.0 / 500.0;

void main() {
    vec2 offsets[9] = vec2[](
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

    float kernel[9];
    if (type == 1) kernel = blur;
    else if (type == 2) kernel = sharpen;
    else if (type == 3) kernel = edge;

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, vTexCoord.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    fragColor = vec4(col, 1.0);
}
