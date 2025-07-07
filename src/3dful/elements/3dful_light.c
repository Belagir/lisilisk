
#include "3dful_core.h"

/**
 * @brief Sets a light's position to some point in 3D space.
 *
 * @param[inout] light
 * @param[in] pos
 */
void light_position(struct light *light, struct vector3 pos)
{
    light->position = pos;
}

/**
 * @brief Sets a light's diffuse strength value : indices 0, 1 and 2 are the color, 3 the strength.
 *
 * @param[inout] light
 * @param[in] diffuse
 */
void light_diffuse(struct light *light, f32 diffuse[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        light->diffuse[i] = diffuse[i];
    }
}

/**
 * @brief Sets a light's specular strength value : indices 0, 1 and 2 are the color, 3 the strength.
 *
 * @param[inout] light
 * @param[in] specular
 */
void light_specular(struct light *light, f32 specular[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        light->specular[i] = specular[i];
    }
}

/**
 * @brief Sets a directional light's direction of rays.
 *
 * @param[inout] light
 * @param[in] direction
 */
void light_directional_direction(struct light_directional *light, vector3 direction)
{
    light->direction = vector3_normalize(direction);
}