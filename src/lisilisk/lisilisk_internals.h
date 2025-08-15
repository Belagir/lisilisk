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

/**
 * @brief Kinds of objects managed by handles.
 *
 */
enum handle_flavor : u8 {
    HANDLE_IS_INVALID = 0,
    HANDLE_REPRESENTS_INSTANCE,
    HANDLE_REPRESENTS_LIGHT_DIREC,
    HANDLE_REPRESENTS_LIGHT_POINT,
    HANDLE_REPRESENTS_CAMERA,
};

enum res_flavor : u8 {
    RES_IS_INVALID = 0,
    RES_REPRESENTS_TEXTURE,
    RES_REPRESENTS_MATERIAL_SHADER,
};

/**
 * @brief Translation of a user-facing handle to the underlying usable data.
 *
 */
union lisk_handle_layout {
    lisk_handle_t full;
    struct {
        /** Hash associated to an object stored in the engine. */
        u32 hash:32;
        /** Eventual instance handle from the 3dful module. */
        u32 internal:HANDLE_BREADTH;
        /** Value from the enum handle_flavor. */
        enum handle_flavor flavor:8;
    };
};

union lisk_res_layout {
    lisk_res_t full;
    struct {
        /** Hash associated to an object stored in the engine. */
        u32 hash:32;
        /** Value from the enum handle_flavor. */
        enum res_flavor flavor:8;
    };
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Data store to cache texture objects.
 *
 */
struct lisilisk_store_texture {
    struct texture *default_texture;
    HASHMAP(struct texture *) textures;
};

/**
 * @brief Data store to cache geometry objects.
 *
 */
struct lisilisk_store_geometry {
    struct geometry *sphere;

    HASHMAP(struct geometry *) geometries;
};

/**
 * @brief Data store to cache material objects.
 *
 */
struct lisilisk_store_material {
    struct lisilisk_store_texture *texture_store;

    struct material *default_material;
    HASHMAP(struct material *) materials;
};

/**
 * @brief Data store to cache model objects.
 *
 */
struct lisilisk_store_model {
    struct lisilisk_store_material *material_store;
    struct lisilisk_store_shader *shader_store;

    HASHMAP(struct model *) models;
};

/**
 * @brief Data store to cache shader objects.
 *
 */
struct lisilisk_store_shader {
    struct shader *default_material_shader;

    HASHMAP(struct shader *) shaders;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Holds data about the OS-facing stuff needed by the engine.
 *
 */
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

struct texture *lisilisk_store_texture_cubemap_cache(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *(*images)[6]);

u32 lisilisk_store_texture_register(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *image);
struct texture *lisilisk_store_texture_retreive(
        struct lisilisk_store_texture *store,
        u32 hash);

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

u32 lisilisk_store_shader_register(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert);

u32 lisilisk_store_shader_material_register(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert);

struct shader *lisilisk_store_shader_retreive(
        struct lisilisk_store_shader *store,
        u32 hash);

#endif
