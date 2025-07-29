
#ifndef LISILISK_STORES_H__
#define LISILISK_STORES_H__

#include <lisilisk.h>
#include <hashmap.h>

#include "../../3dful/3dful.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct lisilisk_geometry_store {
    HASHMAP(struct geometry *) geometries;
};

struct lisilisk_model_store {
    HASHMAP(struct model *) models;
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
struct model *lisilisk_model_store_item(
        struct lisilisk_model_store *store,
        const char *name);
struct model *lisilisk_model_store_retrieve(
        struct lisilisk_model_store *store,
        const char *name);

// -----------------------------------------------------------------------------

#endif
