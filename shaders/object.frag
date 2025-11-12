#version 410 core
in VS_OUT
{
    vec2 TexCoord;
    mat3 TBN;
    vec3 FragPos;
    vec4 FragPosLightSpace;
    vec4 FragPosPersLightSpace;
    vec3 ViewDir;
    vec3 TangentViewDir;
} fs_in;

#include "ub/camera.glsl"
#include "ub/shadow.glsl"
#include "material.glsl"
#include "blinnPhong.glsl"
#include "utils/normalMap.glsl"
#include "utils/parallaxMap.glsl"

out vec4 fragColor;

void main() {
    vec2 texCoord = parallaxMapping(fs_in.TexCoord, fs_in.TangentViewDir, material.heightScale, material.hasHeightMap);
    vec3 N = normalMapping(fs_in.TBN, texCoord, material.hasNormalMap);
    vec3 diffuse = texture(material.texture_diffuse, texCoord).rgb;
    float specular = texture(material.texture_specular, texCoord).a;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    bool hasLights = lightCount.x > 0 || lightCount.y > 0 || lightCount.z > 0;
    vec3 result = mix(diffuse, calculateLights(N, fs_in.FragPos, viewPos.xyz, fs_in.ViewDir, fs_in.FragPosLightSpace, fs_in.FragPosPersLightSpace, diffuse, specular, material.shininess), float(hasLights));

    fragColor = vec4(result, 1.0);
}