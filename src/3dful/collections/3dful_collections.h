
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


#endif
