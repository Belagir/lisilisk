
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

struct object {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
};

struct ogl_target ogl_target_create(const char *name, u32 width, u32 height);
void ogl_target_destroy(struct ogl_target *target);

struct shader shader_compile_vertex(BUFFER *shader_source);
struct shader shader_compile_fragment(BUFFER *shader_source);

void shader_destroy(struct shader shader);

struct object create_object_empty(struct allocator alloc);
void destroy_object(struct allocator alloc, struct object *object);
void wavefront_obj_load(BUFFER *obj_source, struct object *out_object);

#endif