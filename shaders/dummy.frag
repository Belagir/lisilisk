#version 330 core

uniform float AMBIENT_LIGHT_STRENGTH;
uniform vec3 AMBIENT_LIGHT_COLOR;

uniform vec3 COLOR;

in vec3 Normal;

out vec4 FragColor;


void main()
{
    FragColor = vec4(AMBIENT_LIGHT_COLOR * AMBIENT_LIGHT_STRENGTH * Normal, 1.0f);
}
