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
void lisilisk_create_default_texture(
        struct texture *texture);
void lisilisk_default_material(
        struct material *material,
        struct texture *texture);


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_geometry_store {
    HASHMAP(struct geometry *) geometries;
};

struct lisilisk_model_store {
    HASHMAP(struct model *) models;

    struct {
        struct texture blank_texture;
        struct shader material_shader;
        struct material material;
    } defaults;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_geometry_store lisilisk_geometry_store_create(void);
void lisilisk_geometry_store_delete(
        struct lisilisk_geometry_store *store);
struct geometry *lisilisk_geometry_store_item(
        struct lisilisk_geometry_store *store,
        const char *obj_path);

// -----------------------------------------------------------------------------

struct lisilisk_model_store lisilisk_model_store_create(void);
void lisilisk_model_store_delete(
        struct lisilisk_model_store *store);
u32 lisilisk_model_store_item(
        struct lisilisk_model_store *store,
        const char *name);
struct model *lisilisk_model_store_retrieve(
        struct lisilisk_model_store *store,
        u32 hash);

// -----------------------------------------------------------------------------

#endif
