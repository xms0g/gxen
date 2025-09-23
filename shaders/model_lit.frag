#version 410 core

in vec2 vTexcoord;
in vec3 vNormal;
in vec3 vFragPos;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float Kc;
    float Kl;
    float Kq;
};

uniform sampler2D texture_diffuse1;
uniform PointLight pointLight;

out vec4 fragColor;

void main() {
    // ambient
    vec3 ambient = pointLight.ambient;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(pointLight.position - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * pointLight.diffuse;

    float distance = length(pointLight.position - vFragPos);
    float attenuation = 1.0 / (pointLight.Kc + pointLight.Kl * distance + pointLight.Kq * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;

    vec3 texColor = texture(texture_diffuse1, vTexcoord).rgb;
    vec3 result = (ambient + diffuse) * texColor;
    fragColor = vec4(result, 1.0);
}