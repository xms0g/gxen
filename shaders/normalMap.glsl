vec3 normalMapping(mat3 TBN, vec2 texCoord, bool hasNormalMap) {
    vec3 N = normalize(TBN[2]);

    if (hasNormalMap) {
        N = texture(material.texture_normal, texCoord).rgb;
        // transform normal vector to range [-1,1]
        N = N * 2.0 - 1.0;
        // Transform N Tangent space to World space
        N = normalize(TBN * N);
    }

    return N;
}
