
#ifndef OPENGL_SCENES_H__
#define OPENGL_SCENES_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>

#include "../inout/file_operations.h"

struct ogl_target {
    SDL_Window *sdl_window;
    SDL_GLContext *ogl_context;
};

struct shader {
    GLuint shader_handle;
};

union vertex { struct { f32 x, y, z, w; }; f32 array[4u]; };
union color  { struct { f32 r, g, b; }; f32 array[3u]; };

struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(union color) *colors;
};

struct object {
    GLuint vao;

    GLuint vbo[2];

    GLuint shader_program;
};

struct ogl_target ogl_target_create(const char *name, u32 width, u32 height);
void ogl_target_destroy(struct ogl_target *target);

struct shader shader_compile_vertex(BUFFER *shader_source);
struct shader shader_compile_fragment(BUFFER *shader_source);

void shader_destroy(struct shader shader);

struct geometry create_geometry_empty(struct allocator alloc);
void destroy_geometry(struct allocator alloc, struct geometry *geometry);
void wavefront_obj_load_geometry(BUFFER *obj_source, struct geometry *out_geometry);

struct object create_object_from_geometry(struct geometry geometry);
void destroy_object(struct object *object);
void object_set_shaders(struct object *object, struct shader vertex, struct shader frag);

#endif