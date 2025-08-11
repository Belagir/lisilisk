
#include "lisilisk_internals.h"

#include <ustd/res.h>

DECLARE_RES(sphere_object, "res_models_sphere_obj")
DECLARE_RES(quad_object, "res_models_quad_obj")

/**
 * @brief
 *
 * @return struct lisilisk_store_geometry
 */
struct lisilisk_store_geometry lisilisk_store_geometry_create(void)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_store_geometry new_store = { };

    new_store = (struct lisilisk_store_geometry) {
            .sphere = alloc.malloc(alloc, sizeof(*(new_store.sphere))),
            .quad = alloc.malloc(alloc, sizeof(*(new_store.quad))),

            .geometries = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.geometries), 32),
    };

    *new_store.sphere = (struct geometry) { 0 };
    geometry_create(new_store.sphere);
    geometry_wavobj_mem(new_store.sphere, sphere_object_start,
            (size_t) &sphere_object_size);

    *new_store.quad = (struct geometry) { 0 };
    geometry_create(new_store.quad);
    geometry_wavobj_mem(new_store.quad, quad_object_start,
            (size_t) &quad_object_size);

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_store_geometry_delete(
        struct lisilisk_store_geometry *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    geometry_delete(store->sphere);
    alloc.free(alloc, store->sphere);

    geometry_delete(store->quad);
    alloc.free(alloc, store->quad);

    for (size_t i = 0 ; i < array_length(store->geometries) ; i++) {
        geometry_delete(store->geometries[i]);
        alloc.free(alloc, store->geometries[i]);

    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->geometries);

    *store = (struct lisilisk_store_geometry) { };
}

/**
 * @brief
 *
 * @param store
 * @param obj_path
 * @return struct geometry*
 */
struct geometry *lisilisk_store_geometry_cache(
        struct lisilisk_store_geometry *store,
        struct resource_manager *res_manager,
        const char *obj_path)
{
    struct allocator alloc = make_system_allocator();
    size_t pos = 0;
    struct geometry *new_geometry = nullptr;
    const byte *obj_contents = nullptr;
    size_t obj_contents_length = 0;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->geometries, obj_path);

    if (pos < array_length(store->geometries)) {
        return store->geometries[pos];
    }

    // create geometry from file
    new_geometry = alloc.malloc(alloc, sizeof(*new_geometry));
    *new_geometry = (struct geometry) { 0 };
    geometry_create(new_geometry);

    obj_contents = resource_manager_fetch(res_manager, "lisilisk",
            obj_path, &obj_contents_length);

    if (!obj_contents) {
        goto cleanup;
    }

    geometry_wavobj_mem(new_geometry, obj_contents, obj_contents_length);

    // if successful, allocate a new geometry and copy the valid geometry to it
    if (array_length(new_geometry->faces) == 0) {
        goto cleanup;
    }

    hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->geometries, 1);
    pos = hashmap_set(store->geometries, obj_path, &new_geometry);

    store->geometries[pos] = new_geometry;

    return store->geometries[pos];

cleanup:
    geometry_delete(new_geometry);
    alloc.free(alloc, new_geometry);
    return nullptr;
}
