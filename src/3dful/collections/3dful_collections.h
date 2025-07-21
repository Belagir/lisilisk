
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 * Matches OpenGL's GL_TEXTURE_CUBE_MAP_[POSITIVE,NEGATIVE]_[X,Y,Z] order of defines.
 */
enum skybox_face {
    SKYBOX_FACE_RIGHT,
    SKYBOX_FACE_LEFT,
    SKYBOX_FACE_TOP,
    SKYBOX_FACE_BOTTOM,
    SKYBOX_FACE_BACK,
    SKYBOX_FACE_FRONT,

    SKYBOX_FACES_NUMBER,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct scene {
    struct loadable load_state;

    struct model * *models_array;

    struct camera camera;

    struct {
        struct light_point *point_lights_array;
        GLuint ubo_point_lights;
        struct light_directional *direc_lights_array;
        GLuint ubo_dir_lights;
    } light_sources;

    struct {
        struct light ambient_light;
        struct texture *cubemap[SKYBOX_FACES_NUMBER];
    } environment;
};

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera camera);

void scene_light_point(struct scene *scene, struct light_point light);
void scene_light_direc(struct scene *scene, struct light_directional light);
void scene_light_ambient(struct scene *scene, struct light light);

void scene_cubemap(struct scene *scene, struct texture *(*cubemap)[6u]);

void scene_draw(struct scene scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
