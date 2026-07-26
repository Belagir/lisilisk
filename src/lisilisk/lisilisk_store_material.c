
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

    material_emissive(new_store.default_material, (f32[3]) { 1, 1, 1 }, 0 );
    material_emissive_mask(new_store.default_material,
            new_store.texture_store->default_texture);

    material_shininess(new_store.default_material, 64.);
    material_shininess_mask(new_store.default_material,
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
        struct lisilisk_store_texture *store_textures,
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

    hash = hashmap_hash_of(name, 0);
    if (library) {
        hash = hashmap_hash_of(library, hash);
    }

    if (out_hash) {
        *out_hash = hash;
    }

    // try to get the material directly from cache...
    material = lisilisk_store_material_retrieve(store, hash);

    // maybe the material is in a file library the geometry references ?
    if (!material) {
        lisilisk_store_materials_load_from_library(store, store_textures, res_manager, library);
        material = lisilisk_store_material_retrieve(store, hash);
    }

    // still no material, create one from default material then
    if (!material) {
        material = alloc.malloc(alloc, sizeof(*material));
        material_create(material, store->default_material);

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->materials, 1);
        hashmap_set_hashed(store->materials, hash, &material);
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
        struct lisilisk_store_texture *store_textures,
        struct resource_manager *res_manager,
        const char *library)
{
    // allocator shorthand
    struct allocator alloc = make_system_allocator();

    // mtl_contents & mtl_contents_length are for the library file resource
    const byte *mtl_contents = nullptr;
    size_t mtl_contents_length = 0;
    // will contain the raw bytes of the file in an exploitable structure
    ARRAY(byte) mtl_buffer = { 0 };
    // will hold the contents of the file in codebase form
    struct lisilisk_parse_mtl mtl = { };
    PATH relative_path = nullptr;

    // will hold a set of materials to be moved to the store
    HASHMAP(struct material *) parsed_materials = nullptr;

    if (!store || !library) {
        return;
    }

    mtl_contents = resource_manager_fetch(res_manager, "lisilisk",
            library, &mtl_contents_length);
    if (!mtl_contents) {
        return;
    }

    mtl_buffer = array_create(alloc, sizeof(*mtl_buffer), mtl_contents_length);
    lisilisk_parse_mtl_create(&mtl);

    // translate the raw resource to an array of bytes
    array_append_mem(mtl_buffer, mtl_contents, mtl_contents_length);
    // parse the bytes to produce the file contents in the parser intermediate object
    lisilisk_parse_mtl_parse(&mtl, mtl_buffer);

    // construct a relative path from the .mtl file to find textures
    relative_path = path_from_cstring(alloc, library, '/', 2048);
    path_up(relative_path);

    // translate the intermediate object content into a mapped set of allocated materials
    parsed_materials = hashmap_create(make_system_allocator(), sizeof(*parsed_materials), array_length(mtl.materials));
    lisilisk_parse_mtl_to(&mtl, relative_path, store->default_material, &parsed_materials, store_textures, res_manager);

    path_destroy(alloc, &relative_path);
    array_destroy(alloc, (ARRAY_ANY *) &mtl_buffer);
    lisilisk_parse_mtl_destroy(&mtl);

    // move the allocated materials from the hashmap to the store
    for (size_t i = 0 ; i < hashmap_length(parsed_materials) ; i++) {
        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->materials, 1);
        hashmap_set_hashed(store->materials,
                hashmap_hash_of(library, hashmap_keys(parsed_materials)[i]),
                &(parsed_materials[i]));
    }

    hashmap_destroy(make_system_allocator(), (HASHMAP_ANY *) &parsed_materials);
}