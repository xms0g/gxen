struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    vec3 color;
    float shininess;
    bool hasNormalMap;
};

uniform Material material;