#version 410 core

in vec2 vTexcoord;
in vec3 vNormal;
in vec3 vFragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 uLightDir;

out vec4 fragColor;

vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightDir - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 texColor = texture(texture_diffuse1, vTexcoord).rgb;
    vec3 result = (ambient + diffuse) * texColor;
    fragColor = vec4(result, 1.0);
}