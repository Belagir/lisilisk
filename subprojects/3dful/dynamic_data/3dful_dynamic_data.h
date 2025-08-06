/**
 * @file 3dful_dynamic_data.h
 * @author Gabriel Bédat
 * @brief Interface to data utilities that track whether or not the data is
 * loaded to the GPU or not, and sync data from the CPU memory to the GPU
 * depending on the load state.
 *
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef DYNAMIC_DATA_3DFUL_H__
#define DYNAMIC_DATA_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/array.h>

#include <3dful.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// A new user needs the object loaded.
void loadable_add_user(struct loadable *obj);
// An user no longers needs the object loaded.
void loadable_remove_user(struct loadable *obj);
// An user needs the object loaded, and the data is unloaded.
bool loadable_needs_loading(const struct loadable *obj);
// No user needs the object loaded, and the data is loaded.
bool loadable_needs_unloading(const struct loadable *obj);

// -----------------------------------------------------------------------------

// Allocates memory for a new data/handle map that can be loaded.
void handle_buffer_array_create(struct handle_buffer_array *hb_array);
// Deallocate memory used by a data/handle map.
void handle_buffer_array_delete(struct handle_buffer_array *hb_array);

// Assigns some array to the loadable array.
void handle_buffer_array_bind(struct handle_buffer_array *hb_array,
        ARRAY_ANY array);

// Adds a new element at the end of the bound array, syncing if needed.
void handle_buffer_array_push(struct handle_buffer_array *hb_array,
        handle_t *out_handle);
// Removes an element from the bound array, syncing if needed.
void handle_buffer_array_remove(struct handle_buffer_array *hb_array,
        handle_t handle);
// Manually sync an element of the array.
void handle_buffer_array_sync(struct handle_buffer_array *hb_array,
        handle_t handle, size_t offset, size_t size);
// Sets an element of the bound array, syncing if needed.
void handle_buffer_array_set(struct handle_buffer_array *hb_array,
        handle_t handle, void *value, size_t offset, size_t size);

// Query the array to be loaded, if not, to the GPU.
void handle_buffer_array_load(struct handle_buffer_array *hb_array);
// Mark the array as no longer needing to be loaded to the GPU.
void handle_buffer_array_unload(struct handle_buffer_array *hb_array);

#endif
