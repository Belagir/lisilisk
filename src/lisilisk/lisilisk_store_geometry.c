
#include "lisilisk_internals.h"

#include <ustd/res.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

DECLARE_RES(sphere_object, "res_models_sphere_obj")
DECLARE_RES(quad_object, "res_models_quad_obj")

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void lisilisk_store_geometry_load_mem(struct geometry *geometry,
        struct resource_manager *res_manager,
        struct lisilisk_store_material *material_store,
        const byte *obj_buffer, size_t buffer_length, PATH local_path);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

            .geometries = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.geometries), 32),
    };

    *new_store.sphere = (struct geometry) { 0 };
    geometry_create(new_store.sphere);
    lisilisk_store_geometry_load_mem(new_store.sphere, nullptr, nullptr,
            sphere_object_start, (size_t) &sphere_object_size, nullptr);

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
 * @param res_manager
 * @param obj_path
 * @return u32
 */
bool lisilisk_store_geometry_register(
        struct lisilisk_store_geometry *store,
        struct lisilisk_store_material *material_store,
        struct resource_manager *res_manager,
        PATH obj_path,
        u32 *out_hash)
{
    struct allocator alloc = make_system_allocator();
    u32 hash = 0;
    struct geometry *geometry = nullptr;
    const byte *obj_contents = nullptr;
    size_t obj_contents_length = 0;
    PATH directory = nullptr;

    if (!store || !res_manager || !obj_path) {
        return false;
    }

    hash = hashmap_hash_of(obj_path, 0);
    if (out_hash) {
        *out_hash = hash;
    }

    geometry = lisilisk_store_geometry_retrieve(store, hash);
    if (geometry) {
        return true;
    }

    // create geometry from file
    geometry = alloc.malloc(alloc, sizeof(*geometry));
    *geometry = (struct geometry) { 0 };
    geometry_create(geometry);

    obj_contents = resource_manager_fetch(res_manager, "lisilisk",
            obj_path, &obj_contents_length);

    if (!obj_contents) {
        goto cleanup;
    }

    directory = path_from_cstring(alloc, obj_path, '/', 2048);
    path_up(directory);
    lisilisk_store_geometry_load_mem(geometry, res_manager, material_store,
            obj_contents, obj_contents_length, directory);
    path_destroy(alloc, &directory);

    // if successful, allocate a new geometry and copy the valid geometry to it
    if (array_length(geometry->faces) == 0) {
        goto cleanup;
    }

    hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->geometries, 1);
    hashmap_set(store->geometries, obj_path, &geometry);

    return hash;

cleanup:
    geometry_delete(geometry);
    alloc.free(alloc, geometry);
    return 0;
}

/**
 * @brief
 *
 * @param store
 * @param hash
 * @return struct geometry*
 */
struct geometry *lisilisk_store_geometry_retrieve(
        struct lisilisk_store_geometry *store,
        u32 hash)
{
    size_t pos = 0;

    if (!store) {
        return 0;
    }

    pos = hashmap_index_of_hashed(store->geometries, hash);

    if (pos < array_length(store->geometries)) {
        return store->geometries[pos];
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void lisilisk_store_geometry_load_mem(struct geometry *geometry,
        struct resource_manager *res_manager,
        struct lisilisk_store_material *material_store,
        const byte *obj_buffer, size_t buffer_length, PATH local_path)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_parse_obj obj = { };
    ARRAY(byte) buffer = nullptr;

    lisilisk_parse_obj_create(&obj);
    buffer = array_create(alloc, sizeof(*buffer), buffer_length);

    array_append_mem(buffer, obj_buffer, buffer_length);
    lisilisk_parse_obj_parse(&obj, buffer);
    lisilisk_parse_obj_to(&obj, res_manager, material_store, local_path, geometry);

    array_destroy(alloc, (ARRAY_ANY *) &buffer);
    lisilisk_parse_obj_delete(&obj);
}
