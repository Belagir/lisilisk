#version 330 core

out vec4 FragColor;

uniform float AMBIENT_LIGHT_STRENGTH;
uniform vec3 AMBIENT_LIGHT_COLOR;

void main()
{
    FragColor = vec4(AMBIENT_LIGHT_COLOR * AMBIENT_LIGHT_STRENGTH * vec3(1.0f, 0.5f, 0.2f), 1.0f);
}
