
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @return struct lisilisk_store_material
 */
struct lisilisk_store_material lisilisk_store_material_create(
        struct lisilisk_store_texture *texture_store)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_store_material new_store = { };

    new_store = (struct lisilisk_store_material) {
            .texture_store = texture_store,

            .default_material = alloc.malloc(alloc,
                    sizeof(*new_store.default_material)),
            .materials = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.materials), 32),
    };

    *new_store.default_material = (struct material) { 0 };

    material_texture(new_store.default_material,
            new_store.texture_store->default_texture);

    material_ambient(new_store.default_material, (f32[3]) { 1, 1, 1 }, .1 );
    material_ambient_mask(new_store.default_material,
            new_store.texture_store->default_texture);

    material_diffuse(new_store.default_material, (f32[3]) { 1, 1, 1 }, 1.2 );
    material_diffuse_mask(new_store.default_material,
            new_store.texture_store->default_texture);

    material_specular(new_store.default_material, (f32[3]) { 1, 1, 1 }, .6 );
    material_specular_mask(new_store.default_material,
            new_store.texture_store->default_texture);
    material_shininess(new_store.default_material, 128);

    material_emissive(new_store.default_material, (f32[3]) { 1, 1, 1 }, 0 );
    material_emissive_mask(new_store.default_material,
            new_store.texture_store->default_texture);

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_store_material_delete(
        struct lisilisk_store_material *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    alloc.free(alloc, store->default_material);

    for (size_t i = 0 ; i < array_length(store->materials) ; i++) {
        alloc.free(alloc, store->materials[i]);
    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->materials);

    *store = (struct lisilisk_store_material) { };
}

/**
 * @brief
 *
 * @param store
 * @param name
 * @return struct material*
 */
struct material *lisilisk_store_material_cache(
        struct lisilisk_store_material *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    size_t pos = 0;
    struct material *new_material = nullptr;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->materials, name);

    if (pos < array_length(store->materials)) {
        return store->materials[pos];
    }

    new_material = alloc.malloc(alloc, sizeof(*new_material));

    if (new_material) {
        *new_material = *store->default_material;

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->materials, 1);
        pos = hashmap_set(store->materials, name, &new_material);

        store->materials[pos] = new_material;

        return store->materials[pos];
    }

    alloc.free(alloc, new_material);
    return nullptr;
}

/**
 * @brief
 *
 * @param store
 * @return struct material*
 */
struct material *lisilisk_store_material_default(
        struct lisilisk_store_material *store)
{
    if (!store) {
        return nullptr;
    }

    return store->default_material;
}
