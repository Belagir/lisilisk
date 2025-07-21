#version 330 core

in vec3 FragUV;
out vec4 FragColor;

uniform samplerCube Skybox;

void main()
{
    FragColor = texture(Skybox, FragUV);
}
