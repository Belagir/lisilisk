
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
 * @brief Sets a light's color value : indices 0, 1 and 2 are the color's rgb, and 3 the strength.
 *
 * @param[inout] light
 * @param[in] color
 */
void light_color(struct light *light, f32 color[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        light->color[i] = color[i];
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