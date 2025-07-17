
#include <stdarg.h>

#include <SDL2/SDL_image.h>

#include "3dful.h"

/**
 * @brief
 *
 * @param name
 * @param width
 * @param height
 * @return struct application
 */
struct application application_create(const char *name, u32 width, u32 height)
{
    struct application target = {};
    struct logger *log = logger_create(stderr, LOGGER_ON_DESTROY_DO_NOTHING);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logger_log(log, LOGGER_SEVERITY_ERRO, "Failed to initialise SDL:\n%s", SDL_GetError());
        return (struct application) { };
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) != (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
        logger_log(log, LOGGER_SEVERITY_ERRO, "Failed to initialise SDL_image:\n%s", IMG_GetError());
        return (struct application) { };
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    /* Create our window centered at 512x512 resolution */
    target.sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!target.sdl_window) {
        logger_log(log, LOGGER_SEVERITY_CRIT, "Failed to spawn window:\n%s", SDL_GetError());
        return (struct application) { };
    }

    /* Create our opengl context and attach it to our window */
    target.ogl_context = SDL_GL_CreateContext(target.sdl_window);
    if (!target.sdl_window) {
        logger_log(log, LOGGER_SEVERITY_CRIT, "Failed to spawn opengl context:\n%s", SDL_GetError());
        application_destroy(&target);
        return (struct application) { };
    }

    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    return target;
}

/**
 * @brief
 *
 * @param target
 */
void application_destroy(struct application *target)
{
    if (!target) {
        return;
    }

    SDL_GL_DeleteContext(target->ogl_context);
    SDL_DestroyWindow(target->sdl_window);

    IMG_Quit();
    SDL_Quit();
}
