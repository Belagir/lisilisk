/**
 * @file lisilisk_internals.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LISILISK_INTERNALS_H__
#define LISILISK_INTERNALS_H__

#include <lisilisk.h>
#include <ustd/hashmap.h>

#include <3dful.h>
#include <resourceful.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

enum handle_flavor {
    HANDLE_IS_INVALID,
    HANDLE_REPRESENTS_INSTANCE,
    HANDLE_REPRESENTS_LIGHT_DIREC,
    HANDLE_REPRESENTS_LIGHT_POINT,
    HANDLE_REPRESENTS_CAMERA,
};

union lisk_handle_layout {
    lisk_handle_t full;
    struct { lisk_handle_t hash:32, internal:HANDLE_BREADTH, flavor:8; };
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_store_texture {
    struct texture *default_texture;
    HASHMAP(struct texture *) textures;
};

struct lisilisk_store_geometry {
    struct geometry *sphere;
    HASHMAP(struct geometry *) geometries;
};

struct lisilisk_store_material {
    struct lisilisk_store_texture *texture_store;

    struct material *default_material;
    HASHMAP(struct material *) materials;
};

struct lisilisk_store_model {
    struct lisilisk_store_material *material_store;
    struct lisilisk_store_shader *shader_store;

    HASHMAP(struct model *) models;
};

struct lisilisk_store_shader {
    struct shader *default_material_shader;

    HASHMAP(struct shader *) shaders;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_context {
        struct SDL_Window *window;
        SDL_GLContext *opengl;
        struct resource_manager *res_manager;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void lisilisk_setup_environment(
        struct environment *env,
        struct geometry *sky_shape,
        struct shader *sky_shader);
void lisilisk_setup_camera(
        struct camera *camera,
        struct lisilisk_context *context);

// -----------------------------------------------------------------------------

void lisilisk_context_init(
        struct lisilisk_context *context,
        struct logger *log,
        const char *name,
        u32 width, u32 height);

void lisilisk_context_deinit(
        struct lisilisk_context *context);

void lisilisk_context_window_set_size(
        struct lisilisk_context *context,
        u32 width, u32 height);

void lisilisk_context_window_get_size(
        struct lisilisk_context *context,
        i32 *width, i32 *height);

void lisilisk_context_window_set_name(
        struct lisilisk_context *context,
        const char *name);

void lisilisk_context_integrate_resources(
        struct lisilisk_context *context,
        const char *folder);

// -----------------------------------------------------------------------------

struct lisilisk_store_texture lisilisk_store_texture_create(void);
void lisilisk_store_texture_delete(
        struct lisilisk_store_texture *store);

struct texture *lisilisk_store_texture_cache(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *image);
struct texture *lisilisk_store_texture_cubemap_cache(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *(*images)[6]);

// -----------------------------------------------------------------------------

struct lisilisk_store_geometry lisilisk_store_geometry_create(void);
void lisilisk_store_geometry_delete(
        struct lisilisk_store_geometry *store);

struct geometry *lisilisk_store_geometry_cache(
        struct lisilisk_store_geometry *store,
        struct resource_manager *res_manager,
        const char *obj_path);

// -----------------------------------------------------------------------------

struct lisilisk_store_material lisilisk_store_material_create(
        struct lisilisk_store_texture *texture_store);
void lisilisk_store_material_delete(
        struct lisilisk_store_material *store);

struct material *lisilisk_store_material_cache(
        struct lisilisk_store_material *store,
        const char *name);

// -----------------------------------------------------------------------------

struct lisilisk_store_model lisilisk_store_model_create(
        struct lisilisk_store_material *material_store,
        struct lisilisk_store_shader *shader_store);
void lisilisk_store_model_delete(
        struct lisilisk_store_model *store);

u32 lisilisk_store_model_register(
        struct lisilisk_store_model *store,
        const char *name);
struct model *lisilisk_store_model_retrieve(
        struct lisilisk_store_model *store,
        u32 hash);

// -----------------------------------------------------------------------------

struct lisilisk_store_shader lisilisk_store_shader_create(void);
void lisilisk_store_shader_delete(
        struct lisilisk_store_shader *shader_store);
struct shader *lisilisk_store_shader_cache(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert);

#endif
