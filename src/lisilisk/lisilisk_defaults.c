/**
 * @file lisilisk_defaults.c
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "lisilisk_internals.h"

#include <SDL2/SDL_image.h>

#include <ustd/res.h>

/**
 * @brief Loads a default environment for the engine.
 *
 * @param[out] env target environment.
 * @param[in] sky_shape Shaped used to render the sky.
 * @param[in] sky_shader Shader applied to the shape of the sky.
 */
void lisilisk_setup_environment(
        struct environment *env,
        struct geometry *sky_shape,
        struct shader *sky_shader)
{
    if (!env) {
        return;
    }

    *env = (struct environment) { 0 };

    environment_bg(env, (f32[3]) { .2, .2, .2 });
    environment_fog(env, (f32[3]) { 1., .5, 1. }, 1500.);
    environment_ambient(env, (struct light) { { .3, .3, .3, 1. } });
    environment_shader(env, sky_shader);
    environment_geometry(env, sky_shape);
}

/**
 * @brief Configures a default camera for the engine.
 *
 * @param[out] camera Target camera.
 * @param[in] context Context in which the camera evolves.
 */
void lisilisk_setup_camera(
        struct camera *camera,
        struct lisilisk_context *context)
{
    i32 win_w = 0;
    i32 win_h = 0;

    if (!camera || !context) {
        return;
    }

    lisilisk_context_window_get_size(context, &win_w, &win_h);

    if (win_h == 0) {
        return;
    }

    camera_aspect(camera, (f32) win_w / (f32) win_h);
    camera_fov(camera, 45.);
    camera_limits(camera, .1, 1000.);
    camera_position(camera, (struct vector3) { 0, 1, 2 });
    camera_target(camera, VECTOR3_ORIGIN);
}
