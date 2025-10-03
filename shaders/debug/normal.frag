#version 410 core
in GS_OUT {
    vec3 Color;
} fs_in;

out vec4 fragColor;

void main() {
    fragColor = vec4(fs_in.Color, 1.0);
}