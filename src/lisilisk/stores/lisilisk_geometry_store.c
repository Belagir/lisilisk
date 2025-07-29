
#include "lisilisk_stores.h"

/**
 * @brief
 *
 * @return struct lisilisk_geometry_store
 */
struct lisilisk_geometry_store lisilisk_geometry_store_create(void)
{
    struct lisilisk_geometry_store new_store = { };

    new_store = (struct lisilisk_geometry_store) {
            .geometries = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.geometries), 32),
    };

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_geometry_store_delete(
        struct lisilisk_geometry_store *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    for (size_t i = 0 ; i < array_length(store->geometries) ; i++) {
        geometry_delete(store->geometries[i]);
        alloc.free(alloc, store->geometries[i]);

    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->geometries);

    *store = (struct lisilisk_geometry_store) { };
}

/**
 * @brief
 *
 * @param store
 * @param obj_path
 * @return struct geometry*
 */
struct geometry *lisilisk_geometry_store_item(
        struct lisilisk_geometry_store *store,
        const char *obj_path)
{
    struct allocator alloc = make_system_allocator();
    size_t pos = 0;
    struct geometry *new_geometry = nullptr;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->geometries, obj_path);

    if (pos < array_length(store->geometries)) {
        return store->geometries[pos];
    }

    // create geometry from file
    new_geometry = alloc.malloc(alloc, sizeof(*new_geometry));
    geometry_create(new_geometry);
    geometry_wavobj(new_geometry, obj_path);

    // if successful, allocate a new geometry and copy the valid geometry to it
    if (array_length(new_geometry->faces)) {
        hashmap_ensure_capacity(alloc,
                (HASHMAP_ANY *) &store->geometries, 1);
        pos = hashmap_set(store->geometries, obj_path, new_geometry);

        store->geometries[pos] = new_geometry;
    } else {
        geometry_delete(new_geometry);
        alloc.free(alloc, new_geometry);
    }

    return store->geometries[pos];
}
