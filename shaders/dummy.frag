#version 330 core

uniform float AMBIENT_LIGHT_STRENGTH;
uniform vec3 AMBIENT_LIGHT_COLOR;

uniform vec3 POINT_LIGHT_POS;
uniform vec3 CAMERA_POS;

uniform vec3 COLOR;
float specularStrength = 0.5;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

vec3 ambient_light()
{
    return AMBIENT_LIGHT_COLOR * AMBIENT_LIGHT_STRENGTH;
}

vec3 diffuse_light(vec3 lightDir, vec3 norm)
{
    float diff = max(dot(norm, lightDir), 0.0);

    return diff * AMBIENT_LIGHT_COLOR;
}

vec3 specular_light(vec3 lightDir, vec3 norm, vec3 viewPos)
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return specularStrength * spec * AMBIENT_LIGHT_COLOR;
}

void main()
{
    vec3 lightDir = normalize(POINT_LIGHT_POS - FragPos);
    vec3 norm = normalize(Normal);

    vec3 result = COLOR * (ambient_light()
            + diffuse_light(lightDir, norm)
            + specular_light(lightDir, norm, CAMERA_POS));

    FragColor = vec4(result, 1.0f);
}
