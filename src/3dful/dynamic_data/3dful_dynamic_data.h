
#ifndef DYNAMIC_DATA_3DFUL_H__
#define DYNAMIC_DATA_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define LOADABLE_FLAG_NONE   (0x0)
#define LOADABLE_FLAG_LOADED (0x1)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef u16 handle_t;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct loadable {
    u16 flags;
    u16 nb_users;
};

// -----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
struct handle_buffer_array {
    struct loadable load_state;

    void *data_array;
    handle_t *handles_array;
    
    GLuint buffer_name;
    GLenum buffer_usage;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void loadable_add_user(struct loadable *obj);
void loadable_remove_user(struct loadable *obj);
i32 loadable_needs_loading(const struct loadable *obj);
i32 loadable_needs_unloading(const struct loadable *obj);

// -----------------------------------------------------------------------------

void handle_buffer_array_create(struct handle_buffer_array *hb_array);
void handle_buffer_array_delete(struct handle_buffer_array *hb_array);

void handle_buffer_array_bind(struct handle_buffer_array *hb_array, void *array);

void handle_buffer_array_push(struct handle_buffer_array *hb_array, handle_t *out_handle);
void handle_buffer_array_remove(struct handle_buffer_array *hb_array, handle_t handle);
void handle_buffer_array_sync(struct handle_buffer_array *hb_array, handle_t handle, size_t offset, size_t size);
void handle_buffer_array_set(struct handle_buffer_array *hb_array, handle_t handle, void *value, size_t offset, size_t size);

void handle_buffer_array_load(struct handle_buffer_array *hb_array);
void handle_buffer_array_unload(struct handle_buffer_array *hb_array);

#endif
