#version 400 core

in vec3 outFragPos;
in vec3 outNormal;
in vec2 outTexcoord;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

out vec4 fragColor;

void main() {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, outTexcoord).rgb;

    // diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, outTexcoord).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, outTexcoord).rgb;

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}