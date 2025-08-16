
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
    material_create(new_store.default_material);

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

    material_delete(store->default_material);
    alloc.free(alloc, store->default_material);

    for (size_t i = 0 ; i < array_length(store->materials) ; i++) {
        material_delete(store->materials[i]);
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
 * @return u32
 */
u32 lisilisk_store_material_register(
        struct lisilisk_store_material *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    struct material *material = nullptr;
    u32 hash = 0;

    hash = hashmap_hash_of(name, 0);
    material = lisilisk_store_material_retrieve(store, hash);

    if (!material) {
        material = alloc.malloc(alloc, sizeof(*material));
        material_create(material);

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->materials, 1);
        hashmap_set(store->materials, name, &material);
    }

    return hash;
}

/**
 * @brief
 *
 * @param store
 * @param hash
 * @return struct material*
 */
struct material *lisilisk_store_material_retrieve(
        struct lisilisk_store_material *store,
        u32 hash)
{
    size_t pos = 0;

    if (!store) {
        return 0;
    }

    pos = hashmap_index_of_hashed(store->materials, hash);

    if (pos < array_length(store->materials)) {
        return store->materials[pos];
    }

    return nullptr;
}
