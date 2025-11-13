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

    vec3 attenuation;
    bool castShadow;

    vec4 cutOff;
};

#define MAX_DIR_LIGHTS  1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS  4

layout (std140) uniform LightBlock
{
    DirectionalLight dirLights[MAX_DIR_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    ivec4 lightCount;
};

uniform sampler2D shadowMap;
uniform samplerCube shadowCubemap;
uniform sampler2D persShadowMap;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace, vec3 albedo, float specular, float shininess);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos,vec3 viewDir, vec3 albedo, float specular, float shininess);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 fragPosLightSpace, vec3 albedo, float specular, float shininess);
float calculateDirectionalShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float calculateOmnidirectionalShadow(vec3 fragPos, vec4 lightPos, vec3 viewPos);
float calculatePerspectiveShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

vec3 calculateLights(vec3 normal, vec3 fragPos, vec3 viewPos, vec3 viewDir, vec4 fragPosLightSpace, vec4 fragPosPersLightSpace, vec3 albedo, float specular, float shininess) {
    vec3 result = vec3(0.0);

    for (int i = 0; i < lightCount.x; i++) {
        result += calculateDirectionalLight(dirLights[i], normal, viewDir, fragPosLightSpace, albedo, specular, shininess);
    }
    for (int i = 0; i < lightCount.y; i++) {
        result += calculatePointLight(pointLights[i], normal, fragPos, viewPos, viewDir, albedo, specular, shininess);
    }
    for (int i = 0; i < lightCount.z; i++) {
        result += calculateSpotLight(spotLights[i], normal, fragPos, viewDir, fragPosPersLightSpace, albedo, specular, shininess);
    }

    return result;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace, vec3 albedo, float specular, float shininess) {
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    // combine results
    vec3 ambient = light.ambient.rgb * albedo;
    vec3 diffuse = light.diffuse.rgb * diff * albedo;
    vec3 finalSpecular = light.specular.rgb * spec * specular;

    // calculate shadow
    float shadow = calculateDirectionalShadow(fragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + finalSpecular));
    return lighting;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos,vec3 viewDir, vec3 albedo, float specular, float shininess) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

    // combine results
    vec3 ambient = light.ambient.rgb * albedo;
    vec3 diffuse = light.diffuse.rgb * diff * albedo;
    vec3 finalSpecular = light.specular.rgb * spec * specular;

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = calculateOmnidirectionalShadow(fragPos, light.position, viewPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + finalSpecular));
    return lighting;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 fragPosLightSpace, vec3 albedo, float specular, float shininess) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.cutOff.z + light.cutOff.w * distance + light.attenuation.x * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutOff.x - light.cutOff.y;
    float intensity = clamp((theta - light.cutOff.y) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient.rgb * albedo;
    vec3 diffuse = light.diffuse.rgb * diff * albedo;
    vec3 finalSpecular = light.specular.rgb * spec * specular;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadow = calculatePerspectiveShadow(fragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + finalSpecular));
    return lighting;
}

float calculateDirectionalShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if (projCoords.z > 1.0)
        return 0.0;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // PCF
    float shadow = 0.0;
    int samples = 3;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -samples; x <= samples; ++x) {
        for (int y = -samples; y <= samples; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= float((2 * samples + 1) * (2 * samples + 1));

    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[](
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float calculateOmnidirectionalShadow(vec3 fragPos, vec4 lightPos, vec3 viewPos) {
    vec3 fragToLight = fragPos - lightPos.xyz;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / omniFarPlanes.x)) / 25.0;

    for (int i = 0; i < samples; ++i) {
        float closestDepth = texture(shadowCubemap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= omniFarPlanes.x;   // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

float calculatePerspectiveShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    // outside light frustum:
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0)
        return 0.0;
    // simple bias based on normal and light direction (reduces peter-panning)
    float biasLocal = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    // PCF
    float shadow = 0.0;
    int samples = 2;
    vec2 texelSize = 1.0 / textureSize(persShadowMap, 0);
    for (int x = -samples; x <= samples; ++x) {
        for (int y = -samples; y <= samples; ++y) {
            float pcfDepth = texture(persShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - biasLocal > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= float((2 * samples + 1) * (2 * samples + 1));

    return shadow;
}

