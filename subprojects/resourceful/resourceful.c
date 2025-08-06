/**
 * @file resourceful.c
 * @author gabriel ()
 * @brief Implementation file of the resource manager module.
 * @version 0.1
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <ustd/sorting.h>
#include <ustd/hashmap.h>

#include <resourceful.h>

#include "resource_storage/resourceful_storage.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Resource manager information aggregating resource storages objects able to work with single
 * storage files.
 */
struct resource_manager {
    /** Storages objects managing one single storage file each. */
    HASHMAP(struct resource_storage *) storages;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Allocates a new resource manager object, returning a pointer to it.
 *
 * @param[inout] alloc Allocator used to create the object.
 * @return resource_manager *
 */
struct resource_manager *resource_manager_create(allocator alloc)
{
    struct resource_manager *new_res_manager = NULL;

    new_res_manager = alloc.malloc(alloc, sizeof(*new_res_manager));

    if (new_res_manager) {
        *new_res_manager = (struct resource_manager) {
            .storages = hashmap_create(alloc, sizeof(*new_res_manager->storages), 32),
        };
    }

    return new_res_manager;
}

/**
 * @brief Releases memory taken by a resource manager object, nullifying the passed pointer.
 *
 * @param[inout] res_manager Resource manager to deallocate.
 * @param[inout] alloc Alloctor used for the operation.
 */
void resource_manager_destroy(struct resource_manager **res_manager, allocator alloc)
{
    if (!res_manager || !*res_manager) {
        return;
    }

    for (size_t i = 0u ; i < array_length((*res_manager)->storages) ; i++) {
        resource_storage_destroy((*res_manager)->storages + i, alloc);
    }

    hashmap_destroy(alloc, (HASHMAP_ANY *) &((*res_manager)->storages));

    alloc.free(alloc, *res_manager);
    *res_manager = NULL;
}

// -------------------------------------------------------------------------------------------------

#undef resource_manager_touch
/**
 * @brief Checks that a resource can be accessed, and creates a reference for it in memory so it can be loaded and
 * unloaded.
 * Returns true if the resource can be returned in a later call to `resource_manager_fetch()`, provided the storage was
 * loaded (see `resource_manager_add_supplicant()` and `resource_manager_remove_supplicant()`).
 *
 * @param[inout] res_manager Target resource manager that will manage this resource.
 * @param[in] str_storage_path Path to the storage file that stores this resource.
 * @param[in] str_res_path Path to the resource.
 * @param[inout] alloc Allocator used to create a reference to the touched resource.
 * @return bool
 */
bool resource_manager_touch(struct resource_manager *res_manager, const char *str_storage_path,
        const char *str_res_path,
        allocator alloc)
{
    size_t found_storage_index = 0u;
    struct resource_storage *new_storage = NULL;

    if (!res_manager) {
        return false;
    }

    found_storage_index = hashmap_index_of(res_manager->storages, str_storage_path);
    if (found_storage_index == array_length(res_manager->storages)) {
        new_storage = resource_storage_create(str_storage_path, alloc);
        if (new_storage) {
            found_storage_index = hashmap_set(res_manager->storages, str_storage_path, &new_storage);
        }
    }

    return resource_storage_check(res_manager->storages[found_storage_index], str_res_path, alloc);
}

#undef resource_manager_fetch
/**
 * @brief Returns a resource from a storage, provided it exists and was loaded
 * (see `resource_manager_add_supplicant()` and `resource_manager_remove_supplicant()`).
 *
 * @param[in] res_manager Resource storage managing the storage and resource.
 * @param[in] str_storage_path Path to the storage file containing the resource.
 * @param[in] str_res_path Path to the resource file.
 * @param[out] out_size Outgoing value, containing the number of bytes the function returned.
 * @return void *
 */
void *resource_manager_fetch(struct resource_manager *res_manager, const char *str_storage_path,
        const char *str_res_path,
         size_t *out_size)
{
    size_t found_storage_index = 0u;

    if (!res_manager || !str_res_path || !str_storage_path) {
        return NULL;
    }

    found_storage_index = hashmap_index_of(res_manager->storages, str_storage_path);
    if (found_storage_index < array_length(res_manager->storages)) {
        return resource_storage_get(res_manager->storages[found_storage_index], str_res_path, out_size);
    }

    return NULL;
}

#undef resource_manager_add_supplicant
/**
 * @brief Adds an entity as using a storage. If the storage was previously unloaded, it will be loaded to provide this
 * new supplicant with the resources it might want.
 *
 * @param[inout] res_manager Target resource manager.
 * @param[in] str_storage_path Path to the storage file the entity registers to.
 * @param[in] id Some unique id added as supplicant to the resource storage.
 * @param[inout] alloc Allocator used for the eventual resource loading.
 */
void resource_manager_add_supplicant(struct resource_manager *res_manager, const char *str_storage_path, u64 id,
            allocator alloc)
{
    size_t found_storage_index = 0u;

    if (!res_manager || !str_storage_path) {
        return;
    }

    found_storage_index = hashmap_index_of(res_manager->storages, str_storage_path);
    if (found_storage_index < array_length(res_manager->storages)) {
        resource_storage_add_supplicant(res_manager->storages[found_storage_index], id, alloc);
    }
}

/**
 * @brief Removes an entity as a supplicant of a resource manager. If it was the last supplicant, the resource storage
 * is unloaded.
 *
 * @param[inout] res_manager Target resource manager.
 * @param[in] id Identifier withdrawing from requesting the resource storage.
 * @param[inout] alloc Allocator used for the eventual resource unloading.
 */
void resource_manager_remove_supplicant(struct resource_manager *res_manager, u64 id, allocator alloc)
{
    if (!res_manager) {
        return;
    }

    for (size_t i = 0u ; i < array_length(res_manager->storages) ; i++) {
        resource_storage_remove_supplicant(res_manager->storages[i], id, alloc);
    }
}
