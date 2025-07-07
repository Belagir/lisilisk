#version 330 core

// ---------------------------------------------------------
// ---------------------------------------------------------

// Uniform containing the coordiantes of the point of view in world space.
uniform vec3 CAMERA_POS;

// ---------------------------------------------------------
// ---------------------------------------------------------

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material MATERIAL;

// ---------------------------------------------------------

// Mirrors the light struct in the codebase.
struct Light {
    vec3 position;

    vec4 color;
};

struct LightPoint {
    Light base;

    float constant;
    float linear;
    float quadratic;
};

struct LightDirectional {
    Light base;

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

vec4 light_diffuse(vec3 light_dir, Light l_base)
{
    float diff = max(dot(Normal, light_dir), 0.0);
    return l_base.color * (diff * MATERIAL.diffuse);
}

// ---------------------------------------------------------

vec4 light_specular(vec3 light_dir, Light l_base)
{
    vec3 view_dir = normalize(CAMERA_POS - FragPos);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), MATERIAL.shininess);

    return l_base.color * (spec * MATERIAL.specular);
}

// ---------------------------------------------------------
// ---------------------------------------------------------

vec4 light_point_contribution(LightPoint l)
{
    vec3 light_dir = normalize(l.base.position - FragPos);

    vec4 diffuse = light_diffuse(light_dir, l.base);
    vec4 specular = light_specular(light_dir, l.base);

    return diffuse + specular;
}

// ---------------------------------------------------------

vec4 light_directional_contribution(LightDirectional l)
{
    vec3 light_dir = normalize(-l.direction);

    vec4 diffuse = light_diffuse(light_dir, l.base);
    vec4 specular = light_specular(light_dir, l.base);

    return diffuse + specular;
}

// ---------------------------------------------------------
// ---------------------------------------------------------

void main()
{
    vec4 result = vec4(0);

    for (uint i = 0u ; i < LIGHT_POINTS_NB ; i++) {
        result += light_point_contribution(light_points[i]);
    }
    for (uint i = 0u ; i < LIGHT_DIRECTIONALS_NB ; i++) {
        result += light_directional_contribution(light_directionals[i]);
    }

    FragColor = result;
}
