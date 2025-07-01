
#ifndef SCRYING_ORB_H__
#define SCRYING_ORB_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/logging.h>

struct application {
    SDL_Window *sdl_window;
    SDL_GLContext *ogl_context;

    struct logger *log;
};

struct application application_create(const char *name, u32 width, u32 height);
void application_destroy(struct application *target);

#endif
