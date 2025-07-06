
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct scene {
    RANGE(struct object) *objects;

    struct camera camera;

    RANGE(struct light_point) *point_lights;
    GLuint vbo_point_lights;
    RANGE(struct light_directional) *direc_lights;
};

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_object(struct scene *scene, struct object object);
void scene_camera(struct scene *scene, struct camera camera);
void scene_light_point(struct scene *scene, struct light_point light);
void scene_light_direc(struct scene *scene, struct light_directional light);

void scene_draw(struct scene scene);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
