/**
 * @file lisilisk_context.c
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "lisilisk_internals.h"

#include <fts.h>

#include <SDL2/SDL_image.h>

/**
 * @brief Setup the libraries and creates a window to render the opengl
 * context. The window starts hidden.
 *
 * @param[inout] context Context to initialize.
 * @param[in] log Logger to communicate with the outside world.
 * @param[in] name Title of the window.
 * @param[in] width Width, in pixels, of the window.
 * @param[in] height Height, in pixels, of the window.
 */
void lisilisk_context_init(
        struct lisilisk_context *context,
        struct logger *log,
        const char *name,
        u32 width, u32 height)
{

    if (!context) {
        logger_log(log, LOGGER_SEVERITY_ERRO,
                "Improper internal initialisation call.\n");
        return;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logger_log(log, LOGGER_SEVERITY_ERRO,
                "Failed to initialise SDL:\n%s", SDL_GetError());
        return;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)
            != (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
        logger_log(log, LOGGER_SEVERITY_ERRO,
                "Failed to initialise SDL_image:\n%s", IMG_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    /* Create our window centered at 512x512 resolution */
    context->window = SDL_CreateWindow(name,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!context->window) {
        logger_log(log, LOGGER_SEVERITY_CRIT,
                "Failed to spawn window:\n%s", SDL_GetError());
        return;
    }

    /* Create our opengl context and attach it to our window */
    context->opengl = SDL_GL_CreateContext(context->window);
    if (!context->opengl) {
        logger_log(log, LOGGER_SEVERITY_CRIT,
                "Failed to spawn opengl context:\n%s", SDL_GetError());
        return;
    }

    context->res_manager = resource_manager_create(make_system_allocator());

    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
}

/**
 * @brief Quits all libraries and utilities needed for the window.
 * This will close the window.
 *
 * @param[inout] context Context to de-initialize.
 */
void lisilisk_context_deinit(
        struct lisilisk_context *context)
{
    SDL_GL_DeleteContext(context->opengl);
    SDL_DestroyWindow(context->window);
    resource_manager_destroy(&context->res_manager, make_system_allocator());

    context->opengl = nullptr;
    context->window = nullptr;

    IMG_Quit();
    SDL_Quit();
}

/**
 * @brief Changes the size of the window.
 *
 * @param[inout] context Modified context.
 * @param[in] width Width, in pixels, of the window.
 * @param[in] height Height, in pixels, of the window.
 */
void lisilisk_context_window_set_size(
        struct lisilisk_context *context,
        u32 width, u32 height)
{
    SDL_SetWindowSize(context->window, width, height);
}

/**
 * @brief Fills the given pointers with the dimensions of the window.
 *
 * @param[in] context Queried context.
 * @param[out] width Filled with the width, in pixels, of the window.
 * @param[out] height Filled with the height, in pixels, of the window.
 */
void lisilisk_context_window_get_size(
        struct lisilisk_context *context,
        i32 *width, i32 *height)
{
    SDL_GetWindowSize(context->window, width, height);
}

/**
 * @brief Changes the title of the window.
 *
 * @param[inout] context Modified context.
 * @param[in] name String of the new window title.
 */
void lisilisk_context_window_set_name(
        struct lisilisk_context *context,
        const char *name)
{
    SDL_SetWindowTitle(context->window, name);
}

/**
 * @brief Reads all files in a folder, and adds them to the packaged
 * resources system.
 *
 * @param[inout] context Modified context.
 * @param[in] folder Valid path to a system folder.
 */
void lisilisk_context_integrate_resources(
        struct lisilisk_context *context,
        const char *folder)
{
    FTS *hierarchy_stream = fts_open((char *const [])
        {(char *const) folder, nullptr }, FTS_LOGICAL, nullptr);

    FTSENT *entry = nullptr;
    while((entry = fts_read(hierarchy_stream))) {
        if (entry->fts_info == FTS_F) {
            resource_manager_touch(context->res_manager, "lisilisk", entry->fts_path,
                    make_system_allocator());
        }
    }

    fts_close(hierarchy_stream);

    resource_manager_touch(context->res_manager, "lisilisk", nullptr,
            make_system_allocator());
    resource_manager_add_supplicant(context->res_manager, "lisilisk", 0,
            make_system_allocator());
}