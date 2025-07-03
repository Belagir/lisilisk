
#ifndef SCRYING_ORB_CORE_H__
#define SCRYING_ORB_CORE_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/allocation.h>
#include <ustd/common.h>
#include <ustd/math3d.h>
#include <ustd/logging.h>

#include "../3dful.h"
#include "../inout/file_operations.h"

struct shader { GLuint frag_shader, vert_shader, program; };

union vertex { struct { f32 x, y, z, w; }; f32 array[4u]; };
struct face  { u32 indices[3u]; };

struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(struct face) *faces;
};

struct object {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    struct shader *shading;
    struct geometry *geometry;
};

void shader_vert_mem(struct shader *shader, BUFFER *source);
void shader_frag_mem(struct shader *shader, BUFFER *source);
void shader_vert(struct shader *shader, const char *path);
void shader_frag(struct shader *shader, const char *path);
void shader_link(struct shader *shader);
void shader_delete(struct shader *shader);

void geometry_wavobj(struct geometry *geometry, const char *path);
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj);
void geometry_delete(struct geometry *geometry);

void object_geometry(struct object *object, struct geometry *geometry);
void object_shader(struct object *object, struct shader *shader);
void object_load(struct object *object);
void object_unload(struct object *object);
void object_draw(struct object object);

#endif
