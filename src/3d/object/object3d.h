
#ifndef OBJECT3D_H__
#define OBJECT3D_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/allocation.h>
#include <ustd/common.h>

#include "inout/file_operations.h"

struct object {
    GLuint vao;

    GLuint vbo[2];

    GLuint shader_program;
    size_t vertices_nb;
};

struct object create_object(
        struct allocator alloc,
        BUFFER *obj_source,
        BUFFER *vertex_shader_source,
        BUFFER *frag_shader_source);

struct object create_object_from_files(
        struct allocator alloc,
        const char *path_obj,
        const char *path_vertex_shader,
        const char *path_frag_shader);

#endif
