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
#include <hashmap.h>

#include "../3dful/3dful.h"

union lisk_handle_layout {
    lisk_handle_t full;
    struct { u32 hash; u16 reserved; handle_t internal_handle; };
};

void lisilisk_default_environment(
        struct environment *env);
void lisilisk_default_camera(
        struct camera *camera,
        struct SDL_Window *window);

void lisilisk_create_default_material_shader(
        struct shader *shader);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_store_texture {
    struct texture *default_texture;
    HASHMAP(struct texture *) textures;
};

struct lisilisk_store_geometry {
    HASHMAP(struct geometry *) geometries;
};

struct lisilisk_store_material {
    struct lisilisk_store_texture *texture_store;

    struct material *default_material;
    HASHMAP(struct material *) materials;
};

struct lisilisk_store_model {
    struct lisilisk_store_material *material_store;

    HASHMAP(struct model *) models;

    struct {
        struct shader *material_shader;
    } defaults;
};

struct lisilisk_store_shader {
    struct shader *default_material_shader;
    HASHMAP(struct shader *) shaders;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_store_texture lisilisk_store_texture_create(void);
void lisilisk_store_texture_delete(
        struct lisilisk_store_texture *store);

struct texture *lisilisk_store_texture_cache(
        struct lisilisk_store_texture *store,
        const char *name);

// -----------------------------------------------------------------------------

struct lisilisk_store_geometry lisilisk_store_geometry_create(void);
void lisilisk_store_geometry_delete(
        struct lisilisk_store_geometry *store);

struct geometry *lisilisk_store_geometry_cache(
        struct lisilisk_store_geometry *store,
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
        struct lisilisk_store_material *material_store);
void lisilisk_store_model_delete(
        struct lisilisk_store_model *store);

u32 lisilisk_store_model_register(
        struct lisilisk_store_model *store,
        const char *name);
struct model *lisilisk_store_model_retrieve(
        struct lisilisk_store_model *store,
        u32 hash);

#endif
