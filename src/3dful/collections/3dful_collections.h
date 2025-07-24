
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"

// -----------------------------------------------------------------------------
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

/**
 * @brief 
 * 
 */
struct scene {
    struct loadable load_state;

    struct model * *models_array;

    struct camera *camera;

    struct {
        struct light_point *point_lights_array;
        GLuint ubo_point_lights;
        struct light_directional *direc_lights_array;
        GLuint ubo_dir_lights;
    } light_sources;

    struct environment *env;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void handle_buffer_array_create(struct handle_buffer_array *hb_array);
void handle_buffer_array_delete(struct handle_buffer_array *hb_array);

void handle_buffer_array_bind(struct handle_buffer_array *hb_array, void *array);

void handle_buffer_array_push(struct handle_buffer_array *hb_array, handle_t *out_handle);
void handle_buffer_array_remove(struct handle_buffer_array *hb_array, handle_t handle);
void handle_buffer_array_sync(struct handle_buffer_array *hb_array, handle_t handle, size_t offset, size_t size);

void handle_buffer_array_load(struct handle_buffer_array *hb_array);
void handle_buffer_array_unload(struct handle_buffer_array *hb_array);

// -----------------------------------------------------------------------------

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera* camera);
void scene_environment(struct scene *scene, struct environment *env);

void scene_light_point(struct scene *scene, struct light_point light);
void scene_light_direc(struct scene *scene, struct light_directional light);

void scene_draw(struct scene *scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
