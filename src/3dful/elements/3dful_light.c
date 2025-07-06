
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
 * @param diffuse
 */
void light_diffuse(struct light *light, f32 diffuse[4])
{
    light->diffuse[0] = diffuse[0];
    light->diffuse[1] = diffuse[1];
    light->diffuse[2] = diffuse[2];
    light->diffuse[3] = diffuse[3];
}

/**
 * @brief
 *
 * @param light
 * @param specular
 */
void light_specular(struct light *light, f32 specular[4])
{
    light->specular[0] = specular[0];
    light->specular[1] = specular[1];
    light->specular[2] = specular[2];
    light->specular[3] = specular[3];
}

/**
 * @brief
 *
 * @param light
 * @param direction
 */
void light_directional_direction(struct light_directional *light, vector3 direction)
{
    light->direction = vector3_normalize(direction);
}