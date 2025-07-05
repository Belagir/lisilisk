
#include "3dful_core.h"

/**
 * @brief
 *
 * @param light
 * @param transform
 */
void light_position(struct light *light, struct vector3 pos)
{
    light->position = pos;
}

/**
 * @brief
 *
 * @param light
 * @param strength
 */
void light_strength(struct light *light, f32 strength)
{
    light->strength = strength;
}

/**
 * @brief
 *
 * @param light
 * @param diffuse
 */
void light_diffuse(struct light *light, vector3 diffuse)
{
    light->diffuse = diffuse;
}

/**
 * @brief
 *
 * @param light
 * @param specular
 */
void light_specular(struct light *light, vector3 specular)
{
    light->specular = specular;
}
