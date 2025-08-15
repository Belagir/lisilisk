
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
            .default_shader = alloc.malloc(alloc,
                    sizeof(*new_store.default_shader)),
            .shaders = hashmap_create(alloc, sizeof(*new_store.shaders), 32),
    };

    *new_store.default_shader = (struct shader) { 0 };

    shader_material_frag_mem(new_store.default_shader,
            default_fragment_start, (size_t) &default_fragment_size);
    shader_material_vert_mem(new_store.default_shader,
            default_vertex_start, (size_t) &default_vertex_size);

    shader_link(new_store.default_shader);

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

    shader_delete(shader_store->default_shader);
    alloc.free(alloc, shader_store->default_shader);

    for(size_t i = 0 ; i < array_length(shader_store->shaders) ; i++) {
        shader_delete(shader_store->shaders[i]);
        alloc.free(alloc, shader_store->shaders[i]);
    }
    hashmap_destroy(alloc, (HASHMAP_ANY *) &shader_store->shaders);

    *shader_store = (struct lisilisk_store_shader) { 0 };
}

/**
 * @brief
 *
 * @param store
 * @param res_manager
 * @param frag
 * @param vert
 * @return u32
 */
u32 lisilisk_store_shader_material_register(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert)
{
    struct allocator alloc = make_system_allocator();
    u32 hash = 0;
    struct shader *shader = nullptr;
    const byte *vert_source = nullptr;
    size_t vert_source_length = 0;
    const byte *frag_source = nullptr;
    size_t frag_source_length = 0;

    if (!store) {
        return 0;
    }

    hash = hashmap_hash_of(frag, 0);
    hash = hashmap_hash_of(vert, hash);
    shader = lisilisk_store_shader_retreive(store, hash);

    if (!shader) {
        shader = alloc.malloc(alloc, sizeof(*shader));
        *shader = (struct shader) { 0 };

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

        shader_material_frag_mem(shader, frag_source, frag_source_length);
        shader_material_vert_mem(shader, vert_source, vert_source_length);
        shader_link(shader);

        if (shader->program == 0) {
            goto cleanup;
        }

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->shaders, 1);
        hashmap_set_hashed(store->shaders, hash, &shader);
    }

    return hash;

cleanup:
    shader_delete(shader);
    alloc.free(alloc, shader);

    return 0;
}

/**
 * @brief
 *
 * @param store
 * @param hash
 * @return struct shader*
 */
struct shader *lisilisk_store_shader_retreive(
        struct lisilisk_store_shader *store,
        u32 hash)
{
    size_t pos = 0;

    if (!store) {
        return 0;
    }

    pos = hashmap_index_of_hashed(store->shaders, hash);

    if (pos < array_length(store->shaders)) {
        return store->shaders[pos];
    }

    return nullptr;
}
