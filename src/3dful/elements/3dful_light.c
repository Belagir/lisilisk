/**
 * @file 3dful_light.c
 * @author Gabriel Bédat
 * @brief Implementation of light-related procedures.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

/**
 * @brief Sets a light point's position to some point in 3D space.
 *
 * @param[inout] light Modified light point.
 * @param[in] pos New position.
 */
void light_position(struct light_point *light, struct vector3 pos)
{
    light->position = pos;
}

/**
 * @brief Sets a light point's constant attenuation factor.
 *
 * @param[inout] light Modified light point.
 * @param[in] constant Constant attenuation of the light.
 */
void light_point_constant(struct light_point *light, f32 constant)
{
    light->constant = constant;
}

/**
 * @brief Sets a light point's linear attenuation factor.
 *
 * @param[inout] light Modified light point.
 * @param[in] linear Linear attenuation of the light.
 */
void light_point_linear(struct light_point *light, f32 linear)
{
    light->linear = linear;
}

/**
 * @brief Sets a light point's quadratic attenuation factor.
 *
 * @param[inout] light Modified light point.
 * @param[in] quadratic Quadratic attenuation of the light.
 */
void light_point_quadratic(struct light_point *light, f32 quadratic)
{
    light->quadratic = quadratic;
}

/**
 * @brief Sets a light's color value : indices 0, 1 and 2 are the color's rgb,
 * and 3 the strength.
 *
 * @param[inout] light Modified light.
 * @param[in] color New color of the light.
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
 * @param[inout] light Modified directional light.
 * @param[in] direction Light rays direction.
 */
void light_directional_direction(struct light_directional *light,
        vector3 direction)
{
    light->direction = vector3_normalize(direction);
}