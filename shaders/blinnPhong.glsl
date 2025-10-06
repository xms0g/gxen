#include "material.glsl"

struct DirectionalLight {
    vec4 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLight {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation;
};

struct SpotLight {
    vec4 position;
    vec4 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 cutOff;
    vec4 attenuation;
};

#define MAX_DIR_LIGHTS  1
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  4

layout (std140) uniform LightBlock
{
    DirectionalLight dirLights[MAX_DIR_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    ivec4 lightCount;
};

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 calculateLights(vec3 normal, vec3 fragPos, vec4 viewPos) {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos.xyz - fragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < lightCount.x; i++) {
        result += calculateDirectionalLight(dirLights[i], norm, viewDir);
    }
    for (int i = 0; i < lightCount.y; i++) {
        result += calculatePointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
    }
    for (int i = 0; i < lightCount.z; i++) {
        result += calculateSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir);
    }

    return result;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient.rgb * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 diffuse = light.diffuse.rgb * diff * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 specular = light.specular.rgb * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb;

    return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

    // combine results
    vec3 ambient = light.ambient.rgb * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 diffuse = light.diffuse.rgb * diff * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 specular = light.specular.rgb * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb;

    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.cutOff.z + light.cutOff.w * distance + light.attenuation.x * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutOff.x - light.cutOff.y;
    float intensity = clamp((theta - light.cutOff.y) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient.rgb * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 diffuse = light.diffuse.rgb * diff * texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
    vec3 specular = light.specular.rgb * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
