#version 330 core

out vec4 FragColor;

uniform float AMBIENT_LIGHT_STRENGTH;
uniform vec3 AMBIENT_LIGHT_COLOR;

uniform vec3 COLOR;

void main()
{
    FragColor = vec4(AMBIENT_LIGHT_COLOR * AMBIENT_LIGHT_STRENGTH * COLOR, 1.0f);
}
