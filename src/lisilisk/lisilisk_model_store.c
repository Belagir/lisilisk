
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @return struct lisilisk_model_store
 */
struct lisilisk_model_store lisilisk_model_store_create(void)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_model_store new_store = { };

    new_store = (struct lisilisk_model_store) {
            .models = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.models), 32),
            .defaults = {
                .blank_texture = alloc.malloc(alloc,
                        sizeof(*new_store.defaults.blank_texture)),
                .material = alloc.malloc(alloc,
                        sizeof(*new_store.defaults.material)),
                .material_shader = alloc.malloc(alloc,
                        sizeof(*new_store.defaults.material_shader)),
            },
    };

    *new_store.defaults.blank_texture = (struct texture) { 0 };
    *new_store.defaults.material = (struct material) { 0 };
    *new_store.defaults.material_shader = (struct shader) { 0 };

    lisilisk_create_default_texture(
            new_store.defaults.blank_texture);
    lisilisk_create_default_material_shader(
            new_store.defaults.material_shader);
    lisilisk_default_material(new_store.defaults.material,
            new_store.defaults.blank_texture);

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_model_store_delete(
        struct lisilisk_model_store *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    for (size_t i = 0 ; i < array_length(store->models) ; i++) {
        model_delete(store->models[i]);
        alloc.free(alloc, store->models[i]);
    }

    texture_delete(store->defaults.blank_texture);
    shader_delete(store->defaults.material_shader);

    alloc.free(alloc, store->defaults.blank_texture),
    alloc.free(alloc, store->defaults.material),
    alloc.free(alloc, store->defaults.material_shader),

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->models);

    *store = (struct lisilisk_model_store) { 0 };
}

/**
 * @brief
 *
 * @param store
 * @param mesh
 * @return struct model*
 */
u32 lisilisk_model_store_item(
        struct lisilisk_model_store *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    struct model *stored = nullptr;
    u32 model_hash = 0;

    if (!store) {
        return 0;
    }

    model_hash = hashmap_hash_of(name);
    stored = lisilisk_model_store_retrieve(store, model_hash);

    if (stored) {
        return model_hash;
    }

    stored = alloc.malloc(alloc, sizeof(*stored));

    model_create(stored);
    model_material(stored, store->defaults.material);
    model_shader(stored, store->defaults.material_shader);

    hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->models, 1);
    hashmap_set_hashed(store->models, model_hash, &stored);

    return model_hash;
}

/**
 * @brief
 *
 * @param store
 * @param hash
 * @return struct model*
 */
struct model *lisilisk_model_store_retrieve(
        struct lisilisk_model_store *store,
        u32 hash)
{
    size_t pos = 0;

    if (!store) {
        return 0;
    }

    pos = hashmap_index_of_hashed(store->models, hash);

    if (pos < array_length(store->models)) {
        return store->models[pos];
    }

    return nullptr;
}
