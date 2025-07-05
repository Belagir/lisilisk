
#ifndef COLLECTIONS_3DFUL_H__
#define COLLECTIONS_3DFUL_H__

#include "../elements/3dful_core.h"

struct handle {
    u16 challenge;
    u16 index;
};

struct shader_entry {
    u16 challenge;
    struct shader shader;
};

struct shader_store {
    RANGE(struct shader_entry) *shaders;
};

void shader_store_create(struct shader_store *store);
void shader_store_delete(struct shader_store *store);

struct handle shader_store_add(struct shader_store *store, struct shader shader);
void          shader_store_remove(struct shader_store *store, struct handle shader_handle);
struct shader shader_store_get(struct shader_store *store, struct handle shader_handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct scene {
    RANGE(struct object) *objects;
    struct camera camera;

    struct {
        f32 color[3];
        f32 strength;
    } ambient_light;

    struct {
        vector3 pos;
    } point_light;
};

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

void scene_add(struct scene *scene, struct object object);
void scene_camera(struct scene *scene, struct camera camera);
void scene_ambient_light_color(struct scene *scene, f32 color[3]);

void scene_point_light_pos(struct scene *scene, vector3 pos);

void scene_draw(struct scene scene);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

#endif
