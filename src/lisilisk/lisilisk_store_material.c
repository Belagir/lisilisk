
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
    material_create(new_store.default_material, nullptr);

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
bool lisilisk_store_material_register(
        struct lisilisk_store_material *store,
        struct resource_manager *res_manager,
        const char *library,
        const char *name,
        u32 *out_hash)
{
    struct allocator alloc = make_system_allocator();
    struct material *material = nullptr;
    u32 hash = 0;

    if (!store || !name) {
        return false;
    }

    if (library) {
        hash = hashmap_hash_of(library, 0);
    }
    hash = hashmap_hash_of(name, hash);

    if (out_hash) {
        *out_hash = hash;
    }

    // try to get the material directly from cache...
    material = lisilisk_store_material_retrieve(store, hash);

    // maybe the material is in a file library the geometry references ?
    if (!material) {
        lisilisk_store_materials_load_from_library(store, res_manager, library);
        material = lisilisk_store_material_retrieve(store, hash);
    }

    // still no material, create one from default material then
    if (!material) {
        material = alloc.malloc(alloc, sizeof(*material));
        material_create(material, store->default_material);

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->materials, 1);
        hashmap_set(store->materials, name, &material);
    }

    return true;
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

/**
 * @brief
 *
 * @param store
 * @param library
 */
void lisilisk_store_materials_load_from_library(
        struct lisilisk_store_material *store,
        struct resource_manager *res_manager,
        const char *library)
{
    struct allocator alloc = make_system_allocator();

    struct lisilisk_parse_mtl mtl = { };
    ARRAY(byte) mtl_buffer = { 0 };

    const byte *mtl_contents = nullptr;
    size_t mtl_contents_length = 0;

    if (!store || !library) {
        return;
    }

    mtl_contents = resource_manager_fetch(res_manager, "lisilisk",
            library, &mtl_contents_length);
    if (!mtl_contents) {
        return;
    }

    mtl_buffer = array_create(alloc, sizeof(*mtl_buffer), mtl_contents_length);
    array_append_mem(mtl_buffer, mtl_contents, mtl_contents_length);

    lisilisk_parse_mtl_create(&mtl);
    lisilisk_parse_mtl_parse(&mtl, mtl_buffer);


    lisilisk_parse_mtl_destroy(&mtl);

    array_destroy(alloc, (ARRAY_ANY *) &mtl_buffer);
}