
#include "lisilisk_internals.h"

/**
 * @brief
 *
 * @return struct lisilisk_store_texture
 */
struct lisilisk_store_texture lisilisk_store_texture_create(void)
{
    struct allocator alloc = make_system_allocator();
    struct lisilisk_store_texture new_store = { };

    new_store = (struct lisilisk_store_texture) {
            .default_texture = alloc.malloc(alloc,
                    sizeof(*new_store.default_texture)),
            .textures = hashmap_create(
                    make_system_allocator(),
                    sizeof(*new_store.textures), 32),
    };

    texture_2D_default(new_store.default_texture);

    return new_store;
}

/**
 * @brief
 *
 * @param store
 */
void lisilisk_store_texture_delete(
        struct lisilisk_store_texture *store)
{
    struct allocator alloc = make_system_allocator();

    if (!store) {
        return;
    }

    texture_delete(store->default_texture);
    alloc.free(alloc, store->default_texture);

    for (size_t i = 0 ; i < array_length(store->textures) ; i++) {
        texture_delete(store->textures[i]);
        alloc.free(alloc, store->textures[i]);

    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &store->textures);

    *store = (struct lisilisk_store_texture) { };
}

/**
 * @brief
 *
 * @param store
 * @param name
 * @return struct texture*
 */
struct texture *lisilisk_store_texture_cache(
        struct lisilisk_store_texture *store,
        const char *name)
{
    struct allocator alloc = make_system_allocator();
    size_t pos = 0;
    struct texture *new_texture = nullptr;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->textures, name);

    if (pos < array_length(store->textures)) {
        return store->textures[pos];
    }

    new_texture = alloc.malloc(alloc, sizeof(*new_texture));
    texture_2D_file(new_texture, name);

    if (new_texture->specific.image_for_2D) {
        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->textures, 1);
        pos = hashmap_set(store->textures, name, &new_texture);

        store->textures[pos] = new_texture;

        return store->textures[pos];
    }

    texture_delete(new_texture);
    alloc.free(alloc, new_texture);

    return nullptr;
}

/**
 * @brief
 *
 * @param store
 * @return struct texture*
 */
struct texture *lisilisk_store_texture_default(
        struct lisilisk_store_texture *store)
{
    if (!store) {
        return nullptr;
    }

    return store->default_texture;
}
