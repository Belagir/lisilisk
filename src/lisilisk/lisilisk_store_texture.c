
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

    *new_store.default_texture = (struct texture) { 0 };
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
 * @param images
 * @return struct texture*
 */
struct texture *lisilisk_store_texture_cubemap_cache(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *(*images)[6])
{
    struct allocator alloc = make_system_allocator();
    size_t pos = 0;
    struct texture *new_texture = nullptr;
    const byte *obj_contents = nullptr;
    size_t obj_contents_length = 0;

    if (!store) {
        return nullptr;
    }

    pos = hashmap_index_of(store->textures, (*images)[0]);

    if (pos < array_length(store->textures)) {
        return store->textures[pos];
    }

    new_texture = alloc.malloc(alloc, sizeof(*new_texture));
    *new_texture = (struct texture) { 0 };

    for (size_t i = 0 ; i < CUBEMAP_FACES_NUMBER ; i++) {
        obj_contents = resource_manager_fetch(res_manager, "lisilisk",
                (*images)[i], &obj_contents_length);
        if (!obj_contents) {
            continue;
        }
        texture_cubemap_file_mem(new_texture, i, obj_contents,
                obj_contents_length);
    }

    if (new_texture->specific.image_for_2D) {
        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->textures, 1);
        pos = hashmap_set(store->textures, (*images)[0], &new_texture);

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
 * @param res_manager
 * @param image
 * @return u32
 */
u32 lisilisk_store_texture_register(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *image)
{
    struct allocator alloc = make_system_allocator();
    struct texture *texture = nullptr;
    size_t size_image = 0;
    byte *image_buffer = nullptr;
    u32 hash = 0;

    hash = hashmap_hash_of(image, 0);
    texture = lisilisk_store_texture_retreive(store, hash);

    if (!texture) {
        texture = alloc.malloc(alloc, sizeof(*texture));
        *texture = (struct texture) { 0 };

        image_buffer = resource_manager_fetch(res_manager, "lisilisk", image, &size_image);
        texture_2D_file_mem(texture, image_buffer, size_image);

        hashmap_ensure_capacity(alloc, (HASHMAP_ANY *) &store->textures, 1);
        hashmap_set_hashed(store->textures, hash, &texture);
    }

    return hash;
}

/**
 * @brief
 *
 * @param store
 * @param res_manager
 * @param hash
 * @return struct texture*
 */
struct texture *lisilisk_store_texture_retreive(
        struct lisilisk_store_texture *store,
        u32 hash)
{
    size_t pos = 0;

    if (!store) {
        return 0;
    }

    pos = hashmap_index_of_hashed(store->textures, hash);

    if (pos < array_length(store->textures)) {
        return store->textures[pos];
    }

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
