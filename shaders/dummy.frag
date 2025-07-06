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

    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;
};

struct LightDirectional {
    vec3 position;

    vec4 diffuse;
    vec4 specular;

    vec3 direction;
};

#define LIGHT_POINTS_NB_MAX 32
uniform uint LIGHT_POINTS_NB;

layout(std140) uniform BLOCK_LIGHT_POINTS {
    LightPoint light_points[LIGHT_POINTS_NB_MAX];
};

#define LIGHT_DIRECTIONALS_NB_MAX 8
uniform uint LIGHT_DIRECTIONALS_NB;

layout(std140) uniform BLOCK_LIGHT_DIRECTIONALS {
    LightDirectional light_directionals[LIGHT_DIRECTIONALS_NB_MAX];
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
    vec3 diffuse = l.diffuse.a * l.diffuse.xyz * (diff * MATERIAL.diffuse);

    vec3 view_dir = normalize(CAMERA_POS - FragPos);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), MATERIAL.shininess);
    vec3 specular = l.specular.a * l.specular.xyz * (spec * MATERIAL.specular);

    return diffuse + specular;
}

// ---------------------------------------------------------

vec3 light_directional_contribution(LightDirectional l)
{
    vec3 light_dir = normalize(-l.direction);
    float diff = max(dot(Normal, light_dir), 0.0);
    vec3 diffuse = l.diffuse.a * l.diffuse.xyz * (diff * MATERIAL.diffuse);

    return diffuse;
}

// ---------------------------------------------------------
// ---------------------------------------------------------

void main()
{
    vec3 result = vec3(0);

    // for (uint i = 0u ; i < LIGHT_POINTS_NB ; i++) {
    //     result += light_point_contribution(light_points[i]);
    // }
    for (uint i = 0u ; i < LIGHT_DIRECTIONALS_NB ; i++) {
        result += light_directional_contribution(light_directionals[i]);
    }

    FragColor = vec4(result, 1.0f);
}
