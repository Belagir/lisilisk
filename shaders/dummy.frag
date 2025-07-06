#version 330 core

// ---------------------------------------------------------
// ---------------------------------------------------------

uniform vec3 CAMERA_POS;

// ---------------------------------------------------------
// ---------------------------------------------------------

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material MATERIAL;

// ---------------------------------------------------------

struct LightPoint {
    vec3 position;

    float strength;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define LIGHT_POINTS_NB_MAX 32
uniform uint LIGHT_POINTS_NB;

layout(std140) uniform BLOCK_LIGHT_POINTS {
    LightPoint light_points[LIGHT_POINTS_NB_MAX];
};

// ---------------------------------------------------------
// ---------------------------------------------------------

in vec3 Normal;
in vec3 FragPos;

// ---------------------------------------------------------
// ---------------------------------------------------------

out vec4 FragColor;

// ---------------------------------------------------------
// ---------------------------------------------------------

vec3 light_point_contribution(LightPoint l)
{
    vec3 light_dir = normalize(l.position - FragPos);
    float diff = max(dot(Normal, light_dir), 0.0);
    vec3 diffuse = l.strength * l.diffuse * (diff * MATERIAL.diffuse);

    vec3 view_dir = normalize(CAMERA_POS - FragPos);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), MATERIAL.shininess);
    vec3 specular = l.strength * l.specular * (spec * MATERIAL.specular);

    return diffuse + specular;
}

void main()
{
    vec3 result = vec3(0);

    for (uint i = 0u ; i < LIGHT_POINTS_NB ; i++) {
        result += light_point_contribution(light_points[i]);
    }

    FragColor = vec4(result, 1.0f);
}
