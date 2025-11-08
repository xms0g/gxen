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

#include "blinnPhong.glsl"
#include "camera.glsl"
#include "material.glsl"

vec2 parallaxMapping(vec2 texCoord, vec3 viewDir, float heightScale) {
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2  currentTexCoords = texCoord;
    float currentDepthMapValue = texture(material.texture_height, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.texture_height, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.texture_height, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

out vec4 fragColor;

void main() {
    vec3 N = normalize(fs_in.TBN[2]);
    vec2 texCoord = fs_in.TexCoord;

    if (material.hasHeightMap) {
        texCoord = parallaxMapping(fs_in.TexCoord, fs_in.TangentViewDir, material.heightScale);

        if (texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
            discard;
    }

    if (material.hasNormalMap) {
        N = texture(material.texture_normal, texCoord).rgb;
        // transform normal vector to range [-1,1]
        N = N * 2.0 - 1.0;
        // Transform N Tangent space to World space
        N = normalize(fs_in.TBN * N);
    }

    vec3 diffuse = texture(material.texture_diffuse, texCoord).rgb;
    float specular = texture(material.texture_specular, texCoord).a;
    // Create a mask: 0.0 if no lights, 1.0 if at least one light
    bool hasLights = lightCount.x > 0 || lightCount.y > 0 || lightCount.z > 0;
    vec3 result = mix(diffuse, calculateLights(N, fs_in.FragPos, viewPos.xyz, fs_in.ViewDir, fs_in.FragPosLightSpace, fs_in.FragPosPersLightSpace, diffuse, specular, material.shininess), float(hasLights));

    fragColor = vec4(result, 1.0);
}