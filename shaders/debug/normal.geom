#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 Normal;
} gs_in[];

#include "ub/camera.glsl"

const float MAGNITUDE = 0.2;

out GS_OUT {
    vec3 Color;
} gs_out;

void generateLine(int index) {
    gl_Position = projection * gl_in[index].gl_Position;
    gs_out.Color = abs(gs_in[index].Normal);
    EmitVertex();

    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
    gs_out.Color = abs(gs_in[index].Normal);
    EmitVertex();
    EndPrimitive();
}

void main() {
    generateLine(0); // first vertex normal
    generateLine(1); // second vertex normal
    generateLine(2); // third vertex normal
}