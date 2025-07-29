
#include "lisilisk_stores.h"

/**
 * @brief
 *
 * @return struct lisilisk_model_store
 */
struct lisilisk_model_store lisilisk_model_store_create(void)
{
    struct lisilisk_model_store new_store = { };

    new_store = (struct lisilisk_model_store) {
            .models = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.models), 32),
    };

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

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->models);

    *store = (struct lisilisk_model_store) { };
}

/**
 * @brief
 *
 * @param store
 * @param mesh
 * @return struct model*
 */
struct model *lisilisk_model_store_item(
        struct lisilisk_model_store *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    struct model *stored = nullptr;

    if (!store) {
        return nullptr;
    }

    stored = lisilisk_model_store_retrieve(store, name);

    if (stored) {
        return stored;
    }

    stored = alloc.malloc(alloc, sizeof(*stored));
    model_create(stored);

    hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->models, 1);
    hashmap_set(store->models, name, &stored);

    return stored;
}

/**
 * @brief
 *
 * @param store
 * @param name
 * @return struct model*
 */
struct model *lisilisk_model_store_retrieve(
        struct lisilisk_model_store *store,
        const char *name)
{
    size_t pos = 0;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->models, name);

    if (pos < array_length(store->models)) {
        return store->models[pos];
    }

    return nullptr;
}
