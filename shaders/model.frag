#version 410 core

in vec2 vTexcoord;
in vec3 vNormal;
in vec3 vFragPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float Kc;
    float Kl;
    float Kq;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float Kc;
    float Kl;
    float Kq;
};

#define MAX_DIR_LIGHTS  4
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  4

uniform vec3 viewPos;
uniform Material material;

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

out vec4 fragColor;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vFragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < numDirLights; i++) {
        result += calculateDirectionalLight(dirLights[i], norm, viewDir);
    }
    for (int i = 0; i < numPointLights; i++) {
        result += calculatePointLight(pointLights[i], norm, vFragPos, viewDir);
    }
    for (int i = 0; i < numSpotLights; i++) {
        result += calculateSpotLight(spotLights[i], norm, vFragPos, viewDir);
    }

    fragColor = vec4(result, 1.0);
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexcoord).rgb;

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexcoord).rgb;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexcoord).rgb;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}