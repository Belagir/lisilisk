#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

// ---------------------------------------------------------
// ---------------------------------------------------------

// Uniform containing the coordiantes of the point of view in world space.
uniform vec3 CAMERA_POS;

uniform uint TIME;

// ---------------------------------------------------------

layout(std140) uniform BLOCK_MATERIAL {
    vec3 ambient;
    float ambient_strength;

    vec3 diffuse;
    float diffuse_strength;

    vec3 specular;
    float specular_strength;

    vec3 emissive;
    float emissive_strength;

    float shininess;
} MATERIAL;

layout (location = 0) uniform sampler2D ambient_mask;
layout (location = 1) uniform sampler2D specular_mask;
layout (location = 2) uniform sampler2D diffuse_mask;
layout (location = 3) uniform sampler2D emissive_mask;
layout (location = 4) uniform sampler2D base_texture;

// ---------------------------------------------------------
// ---------------------------------------------------------

// Mirrors the light structs in the codebase.

struct Light {
    vec4 color;
};

struct LightPoint {
    vec4 color;

    vec4 position;

    float constant;
    float linear;
    float quadratic;
};

struct LightDirectional {
    vec4 color;
    vec3 direction;
};

// ---------------------------------------------------------

#define LIGHT_POINTS_NB_MAX 32
uniform uint LIGHT_POINTS_NB;

layout(std140) uniform BLOCK_LIGHT_POINTS {
    LightPoint array[LIGHT_POINTS_NB_MAX];
} LIGHT_POINTS;

// ---------------------------------------------------------

#define LIGHT_DIRECTIONALS_NB_MAX 8
uniform uint LIGHT_DIRECTIONALS_NB;

layout(std140) uniform BLOCK_LIGHT_DIRECTIONALS {
    LightDirectional array[LIGHT_DIRECTIONALS_NB_MAX];
} LIGHT_DIRECTIONALS;

// ---------------------------------------------------------

uniform vec4 LIGHT_AMBIENT;

// ---------------------------------------------------------

uniform vec3 FOG_COLOR;
uniform float FOG_DISTANCE;

// ---------------------------------------------------------
// ---------------------------------------------------------

in vec3 Normal;
in vec3 FragPos;
in vec2 FragUV;

// ---------------------------------------------------------
// ---------------------------------------------------------

out vec4 FragColor;

// ---------------------------------------------------------
// ---------------------------------------------------------

vec4 LightContribution;

// ---------------------------------------------------------
// ---------------------------------------------------------

vec4 light_diffuse(vec3 light_dir, vec4 light_color)
{
    float diff = max(dot(Normal, light_dir), 0.0);
    return light_color
            * vec4(diff * MATERIAL.diffuse, 1.)
            * texture(diffuse_mask, FragUV)
            * MATERIAL.diffuse_strength;
}

// ---------------------------------------------------------

vec4 light_specular(vec3 light_dir, vec4 light_color)
{
    vec3 view_dir = normalize(CAMERA_POS - FragPos);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), MATERIAL.shininess);

    return light_color
            * vec4(spec * MATERIAL.specular, 1.)
            * texture(specular_mask, FragUV)
            * MATERIAL.specular_strength;
}

// ---------------------------------------------------------

vec4 light_ambient_contribution(vec4 light_color)
{
    return light_color
            * vec4(MATERIAL.ambient, 1.)
            * texture(ambient_mask, FragUV)
            * MATERIAL.ambient_strength;
}

// ---------------------------------------------------------
// ---------------------------------------------------------

vec4 light_point_contribution(LightPoint l)
{
    vec3 light_dir = normalize(l.position.xyz - FragPos);

    vec4 diffuse = light_diffuse(light_dir, l.color);
    vec4 specular = light_specular(light_dir, l.color);

    float dist = length(l.position.xyz - FragPos);
    float attenuation = 1. / (l.constant + l.linear*dist + l.quadratic*(dist*dist));

    return (diffuse + specular) * attenuation;
}

// ---------------------------------------------------------

vec4 light_directional_contribution(LightDirectional l)
{
    vec3 light_dir = normalize(-l.direction);

    vec4 diffuse = light_diffuse(light_dir, l.color);
    vec4 specular = light_specular(light_dir, l.color);

    return diffuse + specular;
}

// ---------------------------------------------------------

vec4 fog_contribution()
{
    float dist = length(CAMERA_POS - FragPos);
    float tmp = dist/FOG_DISTANCE;

    return vec4(FOG_COLOR, 1.) * (tmp*tmp);
}

// Rest of the shader code is concatenated after this ------
// ---------------------------------------------------------
// ---------------------------------------------------------
// ---------------------------------------------------------
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
