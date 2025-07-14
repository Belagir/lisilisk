
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct scene {
    struct loadable load_state;

    RANGE(struct model *) *models;

    struct camera camera;

    RANGE(struct light_point) *point_lights;
    GLuint ubo_point_lights;
    RANGE(struct light_directional) *direc_lights;
    GLuint ubo_dir_lights;

    struct light ambient_light;
};

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera camera);
void scene_light_point(struct scene *scene, struct light_point light);
void scene_light_direc(struct scene *scene, struct light_directional light);
void scene_light_ambient(struct scene *scene, struct light light);

void scene_draw(struct scene scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
