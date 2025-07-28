
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @param env
 */
void lisilisk_default_environment(
        struct environment *env)
{
    if (!env) {
        return;
    }

    environment_bg(env, (f32[3]) { .2, .2, .2 });
    environment_fog(env, (f32[3]) { .0, .0, .0 }, 2048.);
    environment_ambient(env, (struct light) { { .3, .3, .3, 1. } });
}

/**
 * @brief
 *
 * @param camera
 */
void lisilisk_default_camera(
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

/**
 * @brief
 *
 * @param texture
 */
void lisilisk_create_default_texture(
        struct texture *texture)
{
    if (!texture) {
        return;
    }

    texture_2D_default(texture);
}

/**
 * @brief
 *
 * @param material
 * @param texture
 */
void lisilisk_default_material(
        struct material *material,
        struct texture *texture)
{
    if (!material || !texture) {
        return;
    }

    material_texture(material, texture);

    material_ambient(material, (f32[3]) { 1, 1, 1 }, .1 );
    material_ambient_mask(material, texture);

    material_diffuse(material, (f32[3]) { 1, 1, 1 }, .8 );
    material_diffuse_mask(material, texture);

    material_specular(material, (f32[3]) { 1, 1, 1 }, .8 );
    material_specular_mask(material, texture);
    material_shininess(material, 16);

    material_emissive(material, (f32[3]) { 1, 1, 1 }, 0 );
    material_emissive_mask(material, texture);
}