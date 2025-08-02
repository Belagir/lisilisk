/**
 * @file resourceful_storage.h
 * @author gabriel ()
 * @brief Header file for the module managing single-file resource serialization and deserialization.
 *
 * Create objects linked to a storage data file containing a set of other files' contents, able to load
 * and unload the information contained in it.
 *
 * @version 0.1
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef RESOURCEFUL_STORAGE_H__
#define RESOURCEFUL_STORAGE_H__

#include <ustd/allocation.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define PACKED_RESOURCE_STR_MAX_LEN 256

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to a resource storage object. */
struct resource_storage;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates a new resource storage linked to a file in the filesystem. */
struct resource_storage *resource_storage_create(const char *str_storage_path, struct allocator alloc);

/* Releases memory taken by a resource storage, invalidating all resources sent in the meantime. */
void resource_storage_destroy(struct resource_storage **storage_data, struct allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Tests the presence of a resource (identified by its path) in a resource storage's associated storage file. */
bool resource_storage_check(struct resource_storage *storage_data, const char *str_path, struct allocator alloc);

/* Returns a resource from the loaded resources in a storage. This storage needs to be loaded to return the
   resource (i.e. have at least one supplicant entity.) */
void *resource_storage_get(struct resource_storage *storage_data, const char *str_path, size_t *out_size);

// -------------------------------------------------------------------------------------------------

/* Adds an entity as a supplicant, or user, of a storage. If it is the first one, the storage file will be
  loaded in memory.*/
void resource_storage_add_supplicant(struct resource_storage *storage_data, u64 id, struct allocator alloc);

/* Removes an entity as a supplicant from a storage. If no supplicants are left, the storage unloads its resources. */
void resource_storage_remove_supplicant(struct resource_storage *storage_data, u64 id, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
