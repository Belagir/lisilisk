
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
        struct lisilisk_store_shader *shader_store,
        const char *frag, const char *vert)
{
    (void) shader_store;
    (void) frag;
    (void) vert;

    return nullptr;
}
