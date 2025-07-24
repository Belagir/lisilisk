
#include "3dful_collections.h"

#include <ustd_impl/array_impl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void handle_buffer_array_reload(struct handle_buffer_array *hb_array);
static void handle_buffer_array_sync_capacity(struct handle_buffer_array *hb_array);
static void handle_buffer_array_sync_element(struct handle_buffer_array *hb_array, size_t index, size_t offset, size_t size);

static size_t handle_buffer_array_index_of(struct handle_buffer_array *hb_array, handle_t handle);
static i32 handle_compare(const void *lhs, const void *rhs);


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


/**
 * @brief 
 * 
 * @param hb_array 
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
 * @brief 
 * 
 * @param hb_array 
 */
void handle_buffer_array_delete(struct handle_buffer_array *hb_array)
{
    array_destroy(make_system_allocator(), (void **) hb_array->handles_array);
    *hb_array = (struct handle_buffer_array) { 0 };
}

/**
 * @brief 
 * 
 * @param hb_array 
 * @param array 
 */
void handle_buffer_array_bind(struct handle_buffer_array *hb_array, void *array)
{
    hb_array->data_array = array;
}

/**
 * @brief 
 * 
 * @param hb_array 
 * @param out_handle 
 */
void handle_buffer_array_push(struct handle_buffer_array *hb_array, handle_t *out_handle)
{
    static u16 static_id_counter = 1;

    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (static_id_counter == UINT16_MAX) {
        *out_handle = 0;
        return;
    }

    static_id_counter += 1;
    *out_handle = static_id_counter;

    array_ensure_capacity(make_system_allocator(), (void **) &hb_array->handles_array, 1);
    array_push(hb_array->handles_array, out_handle);
    
    array_ensure_capacity(make_system_allocator(), (void **) &hb_array->data_array, 1);
    target->length += 1; // no need to push just accept garbage at the end

    handle_buffer_array_sync_capacity(hb_array);
}

/**
 * @brief 
 * 
 * @param hb_array 
 * @param handle 
 */
void handle_buffer_array_remove(struct handle_buffer_array *hb_array, handle_t handle)
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
 * @brief 
 * 
 * @param hb_array 
 * @param handle 
 * @param offset 
 * @param size 
 * @param value 
 */
void handle_buffer_array_sync(struct handle_buffer_array *hb_array, handle_t handle, size_t offset, size_t size)
{
    size_t idx = handle_buffer_array_index_of(hb_array, handle);
    handle_buffer_array_sync_element(hb_array, idx, offset, size);
}

/**
 * @brief 
 * 
 * @param hb_array 
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
 * @brief 
 * 
 * @param hb_array 
 */
void handle_buffer_array_unload(struct handle_buffer_array *hb_array)
{
    loadable_remove_user((struct loadable *) hb_array);
    if (loadable_needs_unloading((struct loadable *) hb_array)) {

        glDeleteBuffers(1, &hb_array->buffer_name);
        hb_array->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Synchronizes the buffer object capacity with the data array capacity. 
 * 
 * @param hb_array 
 */
static void handle_buffer_array_sync_capacity(struct handle_buffer_array *hb_array)
{
    GLint buffer_size = 0;

    if (!(hb_array->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
    {
        glGetBufferParameteriv(hb_array->buffer_usage, GL_BUFFER_SIZE, &buffer_size);
    }
    glBindBuffer(hb_array->buffer_usage, 0);
    
    if (buffer_size != (GLint) array_capacity(hb_array->data_array)) {
        handle_buffer_array_reload(hb_array);
    }
}

/**
 * @brief Reloads ONE data element in the data array into the buffer object.
 * 
 * @param hb_array 
 */
static void handle_buffer_array_sync_element(struct handle_buffer_array *hb_array, size_t index, size_t offset, size_t size)
{
    struct array_impl *target = array_impl_of(hb_array->data_array);

    if (!(hb_array->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(hb_array->buffer_usage, hb_array->buffer_name);
    {
        glBufferSubData(hb_array->buffer_usage, 
                (index * target->stride) + offset, size,
                (byte *) hb_array->data_array + index);
    }
    glBindBuffer(hb_array->buffer_usage, 0);
}

/**
 * @brief Reloads ALL the data in the data array into the buffer object.
 * 
 * @param hb_array 
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
static size_t handle_buffer_array_index_of(struct handle_buffer_array *hb_array, handle_t handle)
{
    size_t pos = 0;

    if (array_find(hb_array->handles_array, &handle_compare, &handle, &pos)) {
        return pos;
    }

    return array_length(hb_array->handles_array);
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 handle_compare(const void *lhs, const void *rhs)
{
    handle_t handle_lhs = *(handle_t *) lhs;
    handle_t handle_rhs = *(handle_t *) rhs;

    return (handle_lhs > handle_rhs) - (handle_lhs < handle_rhs);
}
