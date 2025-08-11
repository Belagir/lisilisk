
#include "lisilisk_internals.h"

#include <SDL2/SDL_image.h>

#include <ustd/res.h>

DECLARE_RES(default_fragment, "res_shaders_default_material_frag")
DECLARE_RES(default_vertex,   "res_shaders_default_material_vert")

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

    *env = (struct environment) { 0 };

    environment_bg(env, (f32[3]) { .2, .2, .2 });
    environment_fog(env, (f32[3]) { 1., .5, 1. }, 1500.);
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

    shader_material_frag_mem(shader, default_fragment_start,
            (size_t) &default_fragment_size);
    shader_material_vert_mem(shader, default_vertex_start,
            (size_t) &default_vertex_size);

    shader_link(shader);
}
