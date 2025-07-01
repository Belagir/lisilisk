
#include <stdarg.h>

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

    /* Request an opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    /* Turn on double buffering with a 24bit Z buffer. */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
    SDL_Quit();
}

void application_log_error(struct application *app, enum logger_severity severity, char *format, ...)
{
    va_list args;
    va_start(args, format);

    logger_vlog(app->log, severity, format, args);

    va_end(args);
}


