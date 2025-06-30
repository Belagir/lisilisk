
#ifndef SCRYING_ORB_CORE_H__
#define SCRYING_ORB_CORE_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/allocation.h>
#include <ustd/common.h>

#include "../inout/file_operations.h"

struct shader_program { GLuint frag_shader, vert_shader, program; };

union vertex { struct { f32 x, y, z, w; }; f32 array[4u]; };
union color  { struct { f32 r, g, b; }; f32 array[3u]; };

/**
 * 
 */
struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(union color) *colors;
};

struct object {
    GLuint vao;
    GLuint vbo[2];

    size_t vertice_nb;

    struct shader_program shading;
};

struct shader_program shader_program_create(BUFFER *frag_shader_source, BUFFER *vert_shader_source);
void shader_program_destroy(struct shader_program *shaders);

struct geometry geometry_create_empty(struct allocator alloc);
void geometry_destroy(struct allocator alloc, struct geometry *geometry);
void geometry_from_wavefront_obj(BUFFER *buffer, struct geometry *out_geometry);

struct object object_create(struct geometry geometry, struct shader_program shaders);
void object_destroy(struct object *object);
void object_render(struct object object);

#endif
