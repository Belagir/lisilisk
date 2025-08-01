/**
 * @file 3dful.h
 * @author Gabriel Bédat
 * @brief Main header needed to orchestrate an OpenGL-powered scene.
 * @version 0.1
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef APPLICATION_3DFUL_H__
#define APPLICATION_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/logging.h>

#include "elements/3dful_core.h"
#include "dynamic_data/3dful_dynamic_data.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Links to the window and opengl context. We let SDL manage the
 * OS-related stuff while we stand back in our  comfy chairs.
 *
 */
struct application {
    SDL_Window *sdl_window;
    SDL_GLContext *ogl_context;

    struct logger *log;
};

// -----------------------------------------------------------------------------

/**
 * @brief Holds data about a scene. Models, lights, environment, and camera :
 * all that is needed to compose and render a scene of models to an opengl
 * context.
 *
 */
struct scene {
    struct loadable load_state;

    struct model * *models_array;

    struct camera *camera;

    struct {
        struct light_point *point_lights_array;
        struct handle_buffer_array point_lights;

        struct light_directional *direc_lights_array;
        struct handle_buffer_array direc_lights;
    } light_sources;

    struct environment *env;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

// -----------------------------------------------------------------------------

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera* camera);
void scene_environment(struct scene *scene, struct environment *env);

// -----------------------------------------------------------------------------

void scene_light_point(struct scene *scene, handle_t *out_handle);
void scene_light_point_position(struct scene *scene, handle_t handle,
        struct vector3 pos);
void scene_light_point_color(struct scene *scene, handle_t handle,
        f32 color[4]);
void scene_light_point_attenuation(struct scene *scene, handle_t handle,
        f32 constant, f32 linear, f32 quadratic);
void scene_light_point_remove(struct scene *scene, handle_t handle);

// -----------------------------------------------------------------------------

void scene_light_direc(struct scene *scene, handle_t *out_handle);
void scene_light_direc_orientation(struct scene *scene, handle_t handle,
        struct vector3 dir);
void scene_light_direc_color(struct scene *scene, handle_t handle,
        f32 color[4]);
void scene_light_direc_remove(struct scene *scene, handle_t handle);

// -----------------------------------------------------------------------------

void scene_draw(struct scene *scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct application application_create(const char *name, u32 width, u32 height);
void application_destroy(struct application *target);

#endif
