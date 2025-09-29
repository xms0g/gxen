#include "material.glsl"

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

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 calculateLights(vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

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

    return result;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexCoord).rgb;

    return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexCoord).rgb;

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
    float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, vTexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, vTexCoord).rgb;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
