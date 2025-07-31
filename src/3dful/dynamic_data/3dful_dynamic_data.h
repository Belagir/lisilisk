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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define LOADABLE_FLAG_NONE   (0x0)  ///< Empty flag.
#define LOADABLE_FLAG_LOADED (0x1)  ///< Notifies that the object is loaded.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define HANDLE_BREADTH 24       ///< Bit length of a handle.
#define HANDLE_MAX 0xffffff     ///< Maximum value a handle can take.
typedef u32 handle_t;           ///< Public handle, stored on a type big enough.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief For data storing the load state of some object.
 * This structure only stores data about the load state of other data, and the
 * load procedure is entirely decoupled from functions taking this kind of data.
 * It is the responsability of the user to update this struct's data with
 * loadable_*() functions.
 */
struct loadable {
    /** @brief Load state flags. */
    u16 flags;
    /** @brief Number of times loadable_add_user() has been called. */
    u16 nb_users;
};

// -----------------------------------------------------------------------------

/**
 * @brief For contiguous data that can be loaded to an OpenGL buffer object.
 *
 */
struct handle_buffer_array {
    /** Data about the array load status on the GPU. */
    struct loadable load_state;

    /** Foreign-owned data. MUST be an array as defined in ustd/array.h. */
    ARRAY_ANY data_array;
    /** Owned handle array. MUST be an array as defined in ustd/array.h. */
    ARRAY(handle_t) handles;

    /** OpenGL name for the buffer object. Valid when the data is loaded. */
    GLuint buffer_name;
    /** User-specified buffer object usage. */
    GLenum buffer_usage;
};

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
