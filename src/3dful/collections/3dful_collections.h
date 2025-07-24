
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"
#include "../loading/3dful_loading.h"

// -----------------------------------------------------------------------------
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
        struct handle_buffer_array point_lights;
        // GLuint ubo_point_lights;
        struct light_directional *direc_lights_array;
        GLuint ubo_dir_lights;
    } light_sources;

    struct environment *env;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera* camera);
void scene_environment(struct scene *scene, struct environment *env);

void scene_light_point(struct scene *scene, handle_t *out_handle);
void scene_light_point_position(struct scene *scene, handle_t handle, struct vector3 pos);
void scene_light_point_color(struct scene *scene, handle_t handle, f32 color[4]);
void scene_light_point_attenuation(struct scene *scene, handle_t handle, f32 constant, f32 linear, f32 quadratic);
void scene_light_point_remove(struct scene *scene, handle_t handle);

void scene_light_direc(struct scene *scene, struct light_directional light);

void scene_draw(struct scene *scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
