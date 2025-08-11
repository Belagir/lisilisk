
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @return struct lisilisk_store_model
 */
struct lisilisk_store_model lisilisk_store_model_create(
        struct lisilisk_store_material *material_store,
        struct lisilisk_store_shader *shader_store)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_store_model new_store = { };

    new_store = (struct lisilisk_store_model) {
            .material_store = material_store,
            .shader_store = shader_store,

            .models = hashmap_create(alloc, sizeof(*new_store.models), 32),
    };

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_store_model_delete(
        struct lisilisk_store_model *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    for (size_t i = 0 ; i < array_length(store->models) ; i++) {
        model_delete(store->models[i]);
        alloc.free(alloc, store->models[i]);
    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->models);

    *store = (struct lisilisk_store_model) { 0 };
}

/**
 * @brief
 *
 * @param store
 * @param mesh
 * @return struct model*
 */
u32 lisilisk_store_model_register(
        struct lisilisk_store_model *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    struct model *stored = nullptr;
    u32 model_hash = 0;

    if (!store) {
        return 0;
    }

    model_hash = hashmap_hash_of(name, 0);
    stored = lisilisk_store_model_retrieve(store, model_hash);

    if (stored) {
        return model_hash;
    }

    stored = alloc.malloc(alloc, sizeof(*stored));

    model_create(stored);
    model_material(stored,
            lisilisk_store_material_cache(store->material_store, name));
    model_shader(stored, store->shader_store->default_material_shader);

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
struct model *lisilisk_store_model_retrieve(
        struct lisilisk_store_model *store,
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
