
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @param env
 */
void lisilisk_setup_environment(
        struct environment *env,
        struct geometry *sky_shape,
        struct shader *sky_shader)
{
    if (!env) {
        return;
    }

    environment_bg(env, (f32[3]) { .2, .2, .2 });
    environment_fog(env, (f32[3]) { .0, .0, .0 }, 2048.);
    environment_ambient(env, (struct light) { { .3, .3, .3, 1. } });
    environment_shader(env, sky_shader);
    environment_geometry(env, sky_shape);
}

/**
 * @brief
 *
 * @param camera
 */
void lisilisk_setup_camera(
        struct camera *camera,
        struct SDL_Window *window)
{
    i32 win_w = 0;
    i32 win_h = 0;

    if (!camera || !window) {
        return;
    }

    SDL_GetWindowSize(window, &win_w, &win_h);

    if (win_h == 0) {
        return;
    }

    camera_aspect(camera, (f32) win_w / (f32) win_h);
    camera_fov(camera, 45.);
    camera_limits(camera, .1, 1000.);
    camera_position(camera, (struct vector3) { 0, 1, 2 });
    camera_target(camera, VECTOR3_ORIGIN);
}

/**
 * @brief
 *
 * @param shader
 */
void lisilisk_create_default_material_shader(
        struct shader *shader)
{
    if (!shader) {
        return;
    }

    shader_material_frag(shader, "shaders/user_shaders/material.frag");
    shader_material_vert(shader, "shaders/user_shaders/material.vert");

    shader_link(shader);
}
