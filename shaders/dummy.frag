#version 330 core

uniform vec3 AMBIENT_LIGHT_COLOR;

uniform vec3 POINT_LIGHT_POS;
uniform vec3 CAMERA_POS;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material MATERIAL;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

vec3 ambient_light()
{
    return AMBIENT_LIGHT_COLOR * MATERIAL.ambient;
}

vec3 diffuse_light(vec3 lightDir, vec3 norm)
{
    float diff = max(dot(norm, lightDir), 0.0);

    return (diff * MATERIAL.diffuse) * AMBIENT_LIGHT_COLOR;
}

vec3 specular_light(vec3 lightDir, vec3 norm, vec3 viewPos)
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MATERIAL.shininess);

    return (MATERIAL.specular * spec) * AMBIENT_LIGHT_COLOR;
}

void main()
{
    vec3 lightDir = normalize(POINT_LIGHT_POS - FragPos);
    vec3 norm = normalize(Normal);

    vec3 result = ambient_light()
            + diffuse_light(lightDir, norm)
            + specular_light(lightDir, norm, CAMERA_POS);

    FragColor = vec4(result, 1.0f);
}
