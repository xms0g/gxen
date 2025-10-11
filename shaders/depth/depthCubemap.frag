#version 410 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float omniFarPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / omniFarPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}