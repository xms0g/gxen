#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out VS_OUT
{
    vec3 TexCoord;
} vs_out;

void main() {
    vs_out.TexCoord = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
