/**
 * @file lisilisk.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LISILISK_H__
#define LISILISK_H__

#include <stdint.h>

/** Invalid handle is just full of zeroes. */
#define LISK_HANDLE_NONE ((lisk_handle_t) 0)

/** Handle type to hold basic information and have it be a scalar. */
typedef uint64_t lisk_handle_t;

// Sets the engine ready to be used.
void lisk_init(void);
// Make the engine shut down and release all memory.
void lisk_deinit(void);

// Sets the size of the window, in pixels.
void lisk_resize(
        uint16_t width,
        uint16_t height);
// Sets the title of the window.
void lisk_rename(
        const char *window_name);

// Makes it mossible for a model's instances to be rendered to the scene.
void lisk_model_show(
        const char *name);

// Changes the geometry of a model.
void lisk_model_geometry(
        const char *name,
        const char *obj_file);

void lisk_model_base_texture(
        const char *name,
        const char *texture);

void lisk_model_ambient_color(
        const char *name,
        float (*ambient)[4]);

void lisk_model_ambient_mask(
        const char *name,
        const char *texture_mask);

void lisk_model_diffuse_color(
        const char *name,
        float (*diffuse)[4]);

void lisk_model_diffuse_mask(
        const char *name,
        const char *texture_mask);

void lisk_model_specular_color(
        const char *name,
        float (*specular)[4],
        float shininess);

void lisk_model_specular_mask(
        const char *name,
        const char *texture_mask);

void lisk_model_emission_color(
        const char *name,
        float (*emission)[4]);

void lisk_model_emission_mask(
        const char *name,
        const char *texture_mask);

// Instanciate a model at some point in the world.
lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3],
        float scale);

// Creates a directional light to illuminate the scene.
lisk_handle_t lisk_directional_light_add(
        float (*direction)[3],
        float (*color)[4]);

// Creates a point light to illuminate part of the scene.
lisk_handle_t lisk_point_light_add(
        float (*position)[3],
        float (*color)[4],
        float constant,
        float linear,
        float quadratic);

// Fetches the camera rendering the world.
lisk_handle_t lisk_camera(void);

// Removes a model instance or light from the world.
void lisk_instance_remove(
        lisk_handle_t instance);

// Changes the scale of an instance.
void lisk_instance_set_scale(
        lisk_handle_t instance,
        float scale);

// Changes the position of an instance or a point light.
void lisk_instance_set_position(
        lisk_handle_t instance,
        float (*pos)[3]);

// Changes the orientation of an instance or a directional light.
void lisk_instance_set_rotation(
        lisk_handle_t instance,
        float (*axis)[3],
        float angle_rad);

// Changes the attenuation properties of a light point.
void lisk_instance_light_point_set_attenuation(
        lisk_handle_t instance,
        float constant, float linear, float quadratic);

void lisk_instance_camera_set_fov(
        lisk_handle_t instance,
        float fov);
void lisk_instance_camera_set_limits(
        lisk_handle_t instance,
        float near, float far);
void lisk_instance_camera_set_target(
        lisk_handle_t instance,
        float (*point)[3]);

// Changes the ambient ight setting of the environment.
void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength);

// Changes the skybox to a set of textures.
void lisk_skybox_set(
        const char *(*cubemap)[6]);

// Shows the window and the scene.
void lisk_show(void);

void lisk_draw(void);

void lisk_hide(void);

#endif
