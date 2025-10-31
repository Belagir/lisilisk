/**
 * @file resourceful.h
 * @author gabriel ()
 * @brief Manage resource files through this interface, serving the contained data and registering entities as using
 * storages to automate loading and unloading the resource files' contents.
 *
 * @version 0.1
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RESOURCEFUL_H__
#define RESOURCEFUL_H__

#include <ustd/allocation.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifndef PACKED_RESOURCE_STORAGES_FOLDER
#define PACKED_RESOURCE_STORAGES_FOLDER "program_data"
#endif

#ifndef PACKED_RESOURCE_STORAGES_EXTENSION
#define PACKED_RESOURCE_STORAGES_EXTENSION "data"
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to some data managing resources, their files and entities registered as using those. */
struct resource_manager;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates a resource manager object. */
struct resource_manager *resource_manager_create(allocator alloc);

/* Releases memory taken by a resource manager object, invalidating all resources that were queried. */
void resource_manager_destroy(struct resource_manager **res_manager, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Checks that a resource is accessible in a storage file, and creates a spot for it to be loaded and unloaded. */
bool resource_manager_touch(struct resource_manager *res_manager, const char *str_storage_path,
        const char *str_res_path,
        allocator alloc);
#define resource_manager_touch(manager_, storage_path_, res_path_, alloc_)\
        resource_manager_touch(manager_, \
                PACKED_RESOURCE_STORAGES_FOLDER "/" storage_path_ "."  PACKED_RESOURCE_STORAGES_EXTENSION, \
                res_path_, alloc_)

/* Tries to get a resource from a storage file and returns it. The resource needs to exist and its storage needs to
   have at least one supplicant (to be loaded). */
void *resource_manager_fetch(struct resource_manager *res_manager, const char *str_storage_path,
        const char *str_res_path,
        size_t *out_size);
#define resource_manager_fetch(manager_, storage_path_, res_path_, out_size_)\
        resource_manager_fetch(manager_, \
                PACKED_RESOURCE_STORAGES_FOLDER "/" storage_path_ "."  PACKED_RESOURCE_STORAGES_EXTENSION, \
                res_path_,out_size_)

/* Registers an entity as using a storage, adding it as a supplicant to the storage. If it is the first supplicant, the
   storage is loaded. */
void resource_manager_add_supplicant(struct resource_manager *res_manager, const char *str_storage_path, u64 id,
        allocator alloc);
#define resource_manager_add_supplicant(manager_, storage_path_, res_path_, alloc_)\
        resource_manager_add_supplicant(manager_, \
                PACKED_RESOURCE_STORAGES_FOLDER "/" storage_path_ "."  PACKED_RESOURCE_STORAGES_EXTENSION, \
                res_path_, alloc_)

/* Removes an entity as using a storage. If it was the last supplicant, the storage is unloaded. */
void resource_manager_remove_supplicant(struct resource_manager *res_manager, u64 id, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
