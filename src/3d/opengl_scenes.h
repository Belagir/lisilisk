
#ifndef OPENGL_SCENES_H__
#define OPENGL_SCENES_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <ustd/common.h>

struct ogl_target {
    SDL_Window *sdl_window;
    SDL_GLContext *ogl_context;
};

struct ogl_target ogl_target_create(const char *name, u32 width, u32 height);
void ogl_target_destroy(struct ogl_target *target);

#endif