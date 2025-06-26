
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

struct ogl_target ogl_target_create(const char *name, u32 width, u32 height);
void ogl_target_destroy(struct ogl_target *target);

struct shader shader_compile_vertex(BUFFER *shader_source);
struct shader shader_compile_fragment(BUFFER *shader_source);

void shader_destroy(struct shader shader);

#endif