layout (std140) uniform CameraBlock
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};