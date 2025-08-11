
#include "lisilisk_internals.h"

#include <ustd/res.h>

DECLARE_RES(default_fragment, "res_shaders_default_material_frag")
DECLARE_RES(default_vertex,   "res_shaders_default_material_vert")

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param shader_store
 * @return struct lisilisk_store_shader
 */
struct lisilisk_store_shader lisilisk_store_shader_create(void)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_store_shader new_store = { 0 };

    new_store = (struct lisilisk_store_shader) {
            .default_material_shader = alloc.malloc(alloc,
                    sizeof(*new_store.default_material_shader)),
            .shaders = hashmap_create(alloc, sizeof(*new_store.shaders), 32),
    };

    *new_store.default_material_shader = (struct shader) { 0 };

    shader_material_frag_mem(new_store.default_material_shader,
            default_fragment_start, (size_t) &default_fragment_size);
    shader_material_vert_mem(new_store.default_material_shader,
            default_vertex_start, (size_t) &default_vertex_size);

    shader_link(new_store.default_material_shader);

    return new_store;
}

/**
 * @brief
 *
 * @param shader_store
 */
void lisilisk_store_shader_delete(
        struct lisilisk_store_shader *shader_store)
{
    struct allocator alloc = make_system_allocator();

    if (!shader_store) {
        return;
    }

    shader_delete(shader_store->default_material_shader);
    alloc.free(alloc, shader_store->default_material_shader);

    hashmap_destroy(alloc, (HASHMAP_ANY *) &shader_store->shaders);

    *shader_store = (struct lisilisk_store_shader) { 0 };
}

/**
 * @brief
 *
 * @param shader_store
 * @param frag
 * @param vert
 * @return struct shader*
 */
struct shader *lisilisk_store_shader_cache(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert)
{
    struct allocator alloc = make_system_allocator();
    u32 hash = 0;
    size_t pos = 0;
    struct shader *new_shader = nullptr;
    const byte *vert_source = nullptr;
    size_t vert_source_length = 0;
    const byte *frag_source = nullptr;
    size_t frag_source_length = 0;

    if (!store) {
        return nullptr;
    }

    hash = hashmap_hash_of(frag, 0);
    hash = hashmap_hash_of(vert, hash);

    pos = hashmap_index_of_hashed(store->shaders, hash);

    if (pos < array_length(store->shaders)) {
        return store->shaders[pos];
    }

    new_shader = alloc.malloc(alloc, sizeof(*new_shader));
    *new_shader = (struct shader) { 0 };

    vert_source = resource_manager_fetch(res_manager, "lisilisk",
            vert, &vert_source_length);
    frag_source = resource_manager_fetch(res_manager, "lisilisk",
            frag, &frag_source_length);

    if (!vert_source && !frag_source) {
        goto cleanup;
    }

    if (!vert_source) {
        vert_source = default_vertex_start;
        vert_source_length = (size_t) &default_vertex_size;
    } else if (!frag_source) {
        frag_source = default_fragment_start;
        frag_source_length = (size_t) &default_fragment_size;
    }

    shader_material_frag_mem(new_shader, frag_source, frag_source_length);
    shader_material_vert_mem(new_shader, vert_source, vert_source_length);
    shader_link(new_shader);

    if (new_shader->program == 0) {
        goto cleanup;
    }

    hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->shaders, 1);
    pos = hashmap_set_hashed(store->shaders, hash, &new_shader);

    return store->shaders[pos];

cleanup:
    shader_delete(new_shader);
    alloc.free(alloc, new_shader);

    return nullptr;
}
