#version 410 core
in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} fs_in;

#include "material.glsl"

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(fs_in.Normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.texture_diffuse, fs_in.TexCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.texture_specular, fs_in.TexCoord).r;
}
