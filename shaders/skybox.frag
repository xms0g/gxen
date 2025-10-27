#version 410 core
in VS_OUT
{
    vec3 TexCoord;
} fs_in;

uniform samplerCube skybox;

out vec4 fragColor;

void main() {
    fragColor = texture(skybox, fs_in.TexCoord);
}
