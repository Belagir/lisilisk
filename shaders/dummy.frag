#version 330 core

uniform float AMBIENT_LIGHT_STRENGTH;
uniform vec3 AMBIENT_LIGHT_COLOR;

uniform vec3 POINT_LIGHT_POS;

uniform vec3 COLOR;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(POINT_LIGHT_POS - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * AMBIENT_LIGHT_COLOR;

    vec3 ambient = AMBIENT_LIGHT_COLOR * AMBIENT_LIGHT_STRENGTH;

    vec3 result = (ambient + diffuse) * COLOR;

    FragColor = vec4(result, 1.0f);
}
