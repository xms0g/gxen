struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 color;
    float shininess;
};

uniform Material material;