layout (std140) uniform ShadowBlock
{
    mat4 lightSpaceMatrix;
    mat4 persLightSpaceMatrix;
    vec4 omniFarPlanes;
};
