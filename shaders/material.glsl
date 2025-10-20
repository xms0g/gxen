struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;
    vec3 color;
    float shininess;
    float heightScale;
    bool hasNormalMap;
    bool hasHeightMap;
};

uniform Material material;