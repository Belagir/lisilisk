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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/** Invalid handle is just full of zeroes. */
#define LISK_HANDLE_NONE ((lisk_handle_t) 0)

/** Handle type to a scene object to hold basic information and have it be a
    scalar. */
enum lisk_handle : uint64_t;
typedef enum lisk_handle lisk_handle_t;

/** Invalid handle is just full of zeroes. */
#define LISK_RES_NONE ((lisk_res_t) 0)

/** Handle type to a resource to hold basic information and have it be a
    scalar. */
enum lisk_res : uint64_t;
typedef enum lisk_res lisk_res_t;

// -----------------------------------------------------------------------------

enum lisk_geometry_conf {
    LISK_GEOMETRY_CULL_FRONT,
    LISK_GEOMETRY_CULL_BACK,
    LISK_GEOMETRY_CULL_NONE,
    LISK_GEOMETRY_IN_BACK,
    LISK_GEOMETRY_IN_FRONT,
    LISK_GEOMETRY_IN_SCENE,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Sets the engine ready to be used.
void lisk_init(const char *name, const char *resources_folder);
// Make the engine shut down and release all memory.
void lisk_deinit(void);

// -----------------------------------------------------------------------------

// Sets the size of the window, in pixels.
void lisk_resize(
        uint16_t width,
        uint16_t height);

// Sets the title of the window.
void lisk_rename(
        const char *window_name);

// Requests y=the size of the window.
void lisk_get_size(
        int32_t *width,
        int32_t *height);

// -----------------------------------------------------------------------------

// Loads a texture from a file in the resources directory.
lisk_res_t lisk_texture(
        const char *file);

// Loads a material shader from the resources directory.
lisk_res_t lisk_shader(
        const char *frag_shader,
        const char *vert_shader);

// Creates a material.
lisk_res_t lisk_material(
        const char *name);

// Loads a 3D mesh from a .obj file.
lisk_res_t lisk_geometry(
        const char *obj_file);

// -----------------------------------------------------------------------------

// Changes the value of a uniform in a shader.
void lisk_shader_set_uniform_float(
        lisk_res_t shader,
        const char *uniform_name,
        float value);

// TODO:
void lisk_shader_set_uniform_texture(
        lisk_res_t shader,
        const char *uniform_name,
        lisk_res_t texture);

// -----------------------------------------------------------------------------

// Makes it mossible for a model's instances to be rendered to the scene.
void lisk_model_show(
        const char *name);

// Changes the geometry of a model.
void lisk_model_geometry(
        const char *name,
        lisk_res_t geometry);

// Assigns a material to configure a model's shading.
void lisk_model_material(
        const char *name,
        lisk_res_t material);

// Assigns a material shader to render a model.
void lisk_model_shader(
        const char *name,
        lisk_res_t shader);

// -----------------------------------------------------------------------------

void lisk_geometry_configure(
        lisk_res_t geometry,
        enum lisk_geometry_conf conf);

// -----------------------------------------------------------------------------

// Assigns a base texture to a model.
void lisk_material_base_texture(
        lisk_res_t material,
        lisk_res_t texture);

// Sets how a model is visible in the ambient light.
void lisk_material_ambient(
        lisk_res_t material,
        float (*ambient)[4],
        lisk_res_t texture_mask);

// Sets how a model diffuses the light sources.
void lisk_material_diffuse(
        lisk_res_t material,
        float (*diffuse)[4],
        lisk_res_t texture_mask);

// Sets how a model reflects the light sources.
void lisk_material_specular(
        lisk_res_t material,
        float (*specular)[4],
        float shininess,
        lisk_res_t texture_mask);

// Sets how a model emits color highlights.
void lisk_material_emission(
        lisk_res_t material,
        float (*emission)[4],
        lisk_res_t texture_mask);

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

// TODO: change nomenclature
// Removes a model instance or light from the world.
void lisk_instance_remove(
        lisk_handle_t instance);

// TODO: change nomenclature
// Changes the scale of an instance.
void lisk_instance_set_scale(
        lisk_handle_t instance,
        float (*scale)[3]);

// TODO: change nomenclature
// Changes the position of an instance or a point light.
void lisk_instance_set_position(
        lisk_handle_t instance,
        float (*pos)[3]);

// TODO: change nomenclature
// Changes the orientation of an instance or a directional light.
void lisk_instance_set_rotation(
        lisk_handle_t instance,
        float (*axis)[3],
        float angle_rad);

// TODO: change nomenclature
// Changes the orientation of an instance or a directional light.
void lisk_instance_set_rotation_quaternion(
        lisk_handle_t instance,
        float (*q)[4]);

// TODO: change nomenclature
// Changes the attenuation properties of a light point.
void lisk_instance_light_point_set_attenuation(
        lisk_handle_t instance,
        float constant, float linear, float quadratic);

// TODO: change nomenclature
// Sets the FOV of the camera.
void lisk_instance_camera_set_fov(
        lisk_handle_t instance,
        float fov);

// TODO: change nomenclature
// Sets the near and far planes of the camera.
void lisk_instance_camera_set_limits(
        lisk_handle_t instance,
        float near, float far);

// TODO: change nomenclature
// Sets target point of the camera.
void lisk_instance_camera_set_target(
        lisk_handle_t instance,
        float (*point)[3]);

// -----------------------------------------------------------------------------

// Changes the ambient ight setting of the environment.
void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength);

// Changes the skybox to a set of textures.
void lisk_skybox_set(
        const char *(*cubemap)[6]);

// Changes the background color, when there is no skybox.
void lisk_bg_color_set(
        float (*color)[3]);

// -----------------------------------------------------------------------------

// Shows the window and the scene.
void lisk_show(void);

// Renders the current scene.
void lisk_draw(void);

// Hides the window.
void lisk_hide(void);

// -----------------------------------------------------------------------------

#endif
