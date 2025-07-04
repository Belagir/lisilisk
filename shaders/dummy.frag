#version 330 core

out vec4 FragColor;

uniform float ambient_light_strength;
uniform vec3 ambient_light_color;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
