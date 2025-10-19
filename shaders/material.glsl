struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    vec3 color;
    float shininess;
    bool hasNormalMap;
};

uniform Material material;