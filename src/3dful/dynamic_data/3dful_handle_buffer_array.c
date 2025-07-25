/**
 * @file 3dful_handle_buffer_array.c
 * @author Gabriel Bédat
 * @brief Handles/data map implementation.
 * Everything related to the struct handle_buffer_array.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "3dful_dynamic_data.h"

#include <ustd_impl/array_impl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void handle_buffer_array_reload(struct handle_buffer_array *hb_array);
static void handle_buffer_array_sync_capacity(
        struct handle_buffer_array *hb_array);
static void handle_buffer_array_sync_element(
        struct handle_buffer_array *hb_array, size_t index,
        size_t offset, size_t size);

static size_t handle_buffer_array_index_of(
            struct handle_buffer_array *hb_array, handle_t handle);
static i32 handle_compare(const void *lhs, const void *rhs);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


/**
 * @brief Allocates memory for a handles/data map.
 * The object produced is not yet usable. It still needs a bound array and
 * an OpenGL usage.
 *
 * @param[out] hb_array (overwritten) target array map.
 */
void handle_buffer_array_create(struct handle_buffer_array *hb_array)
{
    *hb_array = (struct handle_buffer_array) {
            .load_state = { 0 },

            .data_array = nullptr,
            .handles_array = array_create(make_system_allocator(),
                    sizeof(*hb_array->handles_array), 32),
            .buffer_name = 0,
            .buffer_usage = GL_NONE,
    };
}

/**
 * @brief Releases memory allocated for a handles/data map.
 * The procedure will not check if the array is currently loaded.
 * Be sure to unload the array before calling this procedure, as the
 * object will become unusable.
 *
 * @param[inout] hb_array Deleted object.
 */
void handle_buffer_array_delete(struct handle_buffer_array *hb_array)
{
    array_destroy(make_system_allocator(), (void **) &hb_array->handles_array);
    *hb_array = (struct handle_buffer_array) { 0 };
}

/**
 * @brief Supplies an array to work with some handles/data map. This may replace
 * an existing array, but handles will not be replaced or deleted.
 *
 * @warning The array may be reallocated and the original pointer invalidated !
 *
 * @warning Only use an array made from the ustd/array.h header. You _will_
 * get segfaults if not !
 *
 * @param[inout] hb_array Target array.
 * @param[in] array Newly bound array.
 */
void handle_buffer_array_bind(struct handle_buffer_array *hb_array, void *array)
{
    hb_array->data_array = array;
    handle_buffer_array_reload(hb_array);
}

/**
 * @brief Adds a new empty element at the end of the bound array, and assigns an
 * handle to this new item. Calling this may trigger a reallocation of the bound
 * array (watch out !!!), of the handle data, and a reload of the data to GPU.
 *
 * @param[inout] hb_array Target array.
 * @param[out] out_handle (needed) Outgoing handle.
 */
void handle_buffer_array_push(struct handle_buffer_array *hb_array,
        handle_t *out_handle)
{
    static u16 static_id_counter = 1;

    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (static_id_counter == UINT16_MAX) {
        *out_handle = 0;
        return;
    }

    *out_handle = static_id_counter;
    static_id_counter += 1;

    array_ensure_capacity(make_system_allocator(),
            (void **) &hb_array->handles_array, 1);
    array_push(hb_array->handles_array, out_handle);

    array_ensure_capacity(make_system_allocator(),
            (void **) &hb_array->data_array, 1);
    // no need to push something just accept garbage at the end
    target->length += 1;

    handle_buffer_array_sync_capacity(hb_array);
}

/**
 * @brief Removes an element previously added to the array.
 * The removal procedure is not stable. The order of elements is not guaranteed
 * to stay the same after removing an element.
 *
 * @param[inout] hb_array Target array.
 * @param[in] handle Handle to the removed element.
 */
void handle_buffer_array_remove(struct handle_buffer_array *hb_array,
        handle_t handle)
{
    size_t idx = handle_buffer_array_index_of(hb_array, handle);

    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (idx == array_length(hb_array->handles_array)) {
        return;
    }

    array_remove_swapback(hb_array->handles_array, idx);
    array_remove_swapback(hb_array->data_array, idx);

    handle_buffer_array_sync_element(hb_array, idx, 0, target->stride);
}

/**
 * @brief Manually synchronize part (or the entirety) of an element. This means
 * that if the array is marked as loaded, the data corresponding to the handle
 * is rewritten to the GPU.
 *
 * Note that handle_buffer_array_set(), using handle_buffer_array_push(), or
 * handle_buffer_array_remove(), this call is made automatically.
 *
 * @param[inout] hb_array Target array.
 * @param[in] handle Handle to the target element.
 * @param[in] offset Offset in the element, in bytes.
 * @param[in] size Size of the data re-written, in bytes.
 */
void handle_buffer_array_sync(struct handle_buffer_array *hb_array,
        handle_t handle, size_t offset, size_t size)
{
    size_t idx = handle_buffer_array_index_of(hb_array, handle);
    if (idx == array_length(hb_array->data_array)) {
        return;
    }

    handle_buffer_array_sync_element(hb_array, idx, offset, size);
}

/**
 * @brief Sets part (or the entirety) of an element. This will automatically
 * synchronize the element in GPU memory if needed.
 * *
 * @param[inout] hb_array Target array.
 * @param[in] handle Handle to the modified element.
 * @param[in] value Pointer to some data of at least `size` bytes.
 * @param[in] offset Offset of the written data in the element, in bytes.
 * @param[in] size Size of the written the data re-written, in bytes.
 */
void handle_buffer_array_set(struct handle_buffer_array *hb_array,
        handle_t handle, void *value, size_t offset, size_t size)
{
    size_t idx = handle_buffer_array_index_of(hb_array, handle);
    if (idx == array_length(hb_array->data_array)) {
        return;
    }

    struct array_impl *target = array_impl_of(hb_array->data_array);

    bytewise_copy((byte *) hb_array->data_array
            + (idx * target->stride) + offset, value, size);

    handle_buffer_array_sync_element(hb_array, idx, offset, size);
}

/**
 * @brief Query the array to be loaded. If it already was, the new user is
 * counted. If it wasn't, the contents of the bound array will be written to
 * the GPU memory, accessible with the handle_buffer_array::buffer_name field.
 *
 * @param[inout] hb_array Loaded array.
 */
void handle_buffer_array_load(struct handle_buffer_array *hb_array)
{
    struct array_impl *target = array_impl_of(hb_array->data_array);

    loadable_add_user((struct loadable *) hb_array);
    if (loadable_needs_loading((struct loadable *) hb_array)) {

        glGenBuffers(1, &hb_array->buffer_name);
        glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
        {
            glBufferData(hb_array->buffer_usage,
                    array_capacity(hb_array->data_array) * target->stride,
                    hb_array->data_array, GL_DYNAMIC_DRAW);
        }
        glBindBuffer(hb_array->buffer_usage, 0);

        hb_array->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief Notifies that the array is no longer used by some other object.
 * If it was the last one, the array is unloaded from GPU data.
 *
 * @param[inout] hb_array Unloaded array.
 */
void handle_buffer_array_unload(struct handle_buffer_array *hb_array)
{
    loadable_remove_user((struct loadable *) hb_array);
    if (loadable_needs_unloading((struct loadable *) hb_array)) {

        glDeleteBuffers(1, &hb_array->buffer_name);
        hb_array->buffer_name = 0;

        hb_array->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Synchronizes the buffer object capacity with the data array capacity.
 *
 * @param[inout] hb_array
 */
static void handle_buffer_array_sync_capacity(
        struct handle_buffer_array *hb_array)
{
    GLint buffer_size = 0;

    if (!(hb_array->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
    {
        glGetBufferParameteriv(hb_array->buffer_usage, GL_BUFFER_SIZE,
                &buffer_size);
    }
    glBindBuffer(hb_array->buffer_usage, 0);

    if (buffer_size != (GLint) array_capacity(hb_array->data_array)) {
        handle_buffer_array_reload(hb_array);
    }
}

/**
 * @brief Reloads ONE data element in the data array into the buffer object.
 *
 * @param[inout] hb_array
 */
static void handle_buffer_array_sync_element(
        struct handle_buffer_array *hb_array, size_t index,
        size_t offset, size_t size)
{
    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (!(hb_array->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
    {
        glBufferSubData(hb_array->buffer_usage,
                (index * target->stride) + offset, size,
                (byte *) hb_array->data_array + (index * target->stride)
                + offset);
    }
    glBindBuffer(hb_array->buffer_usage, 0);
}

/**
 * @brief Reloads ALL the data in the data array into the buffer object.
 *
 * @param[inout] hb_array
 */
static void handle_buffer_array_reload(struct handle_buffer_array *hb_array)
{
    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (!(hb_array->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
    {
        glBufferData(hb_array->buffer_usage,
                target->capacity * target->stride,
                hb_array->data_array, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(hb_array->buffer_usage, 0);
}

/**
 * @brief
 *
 * @param model
 * @param handle
 * @return size_t
 */
static size_t handle_buffer_array_index_of(
        struct handle_buffer_array *hb_array, handle_t handle)
{
    size_t pos = 0;

    if (array_find(hb_array->handles_array, &handle_compare, &handle, &pos)) {
        return pos;
    }

    return array_length(hb_array->handles_array);
}

/**
 * @brief Compares two handles by value.
 * Returns -1, 0 or 1 if lesser, equal, or greater respectivelly.
 *
 * @param[in] lhs
 * @param[in] rhs
 * @return i32
 */
static i32 handle_compare(const void *lhs, const void *rhs)
{
    handle_t handle_lhs = *(handle_t *) lhs;
    handle_t handle_rhs = *(handle_t *) rhs;

    return (handle_lhs > handle_rhs) - (handle_lhs < handle_rhs);
}
