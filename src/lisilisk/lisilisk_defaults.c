
#include "lisilisk_internals.h"

#include <SDL2/SDL_image.h>

#include <ustd/res.h>

DECLARE_RES(default_fragment, "res_shaders_default_material_frag")
DECLARE_RES(default_vertex,   "res_shaders_default_material_vert")

/**
 * @brief
 *
 * @param window
 * @param context
 * @param name
 * @param width
 * @param height
 */
void lisilisk_init_context(
        struct SDL_Window **window,
        SDL_GLContext **context,
        struct resource_manager **res_manager,
        const char *name,
        u32 width, u32 height)
{
    struct logger *log = logger_create(stderr, LOGGER_ON_DESTROY_DO_NOTHING);

    if (!context || !window) {
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
    *window = SDL_CreateWindow(name,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!*window) {
        logger_log(log, LOGGER_SEVERITY_CRIT,
                "Failed to spawn window:\n%s", SDL_GetError());
        return;
    }

    /* Create our opengl context and attach it to our window */
    *context = SDL_GL_CreateContext(*window);
    if (!*context) {
        logger_log(log, LOGGER_SEVERITY_CRIT,
                "Failed to spawn opengl context:\n%s", SDL_GetError());
        return;
    }

    *res_manager = resource_manager_create(make_system_allocator());

    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
}

/**
 * @brief
 *
 * @param window
 * @param context
 */
void lisilisk_deinit_context(
        struct SDL_Window **window,
        SDL_GLContext **context,
        struct resource_manager **res_manager)
{
    SDL_GL_DeleteContext(*context);
    SDL_DestroyWindow(*window);
    resource_manager_destroy(res_manager, make_system_allocator());

    *context = nullptr;
    *window = nullptr;

    IMG_Quit();
    SDL_Quit();
}

/**
 * @brief
 *
 * @param env
 */
void lisilisk_setup_environment(
        struct environment *env,
        struct geometry *sky_shape,
        struct shader *sky_shader)
{
    if (!env) {
        return;
    }

    *env = (struct environment) { 0 };

    environment_bg(env, (f32[3]) { .2, .2, .2 });
    environment_fog(env, (f32[3]) { 1., .5, 1. }, 1500.);
    environment_ambient(env, (struct light) { { .3, .3, .3, 1. } });
    environment_shader(env, sky_shader);
    environment_geometry(env, sky_shape);
}

/**
 * @brief
 *
 * @param camera
 */
void lisilisk_setup_camera(
        struct camera *camera,
        struct SDL_Window *window)
{
    i32 win_w = 0;
    i32 win_h = 0;

    if (!camera || !window) {
        return;
    }

    SDL_GetWindowSize(window, &win_w, &win_h);

    if (win_h == 0) {
        return;
    }

    camera_aspect(camera, (f32) win_w / (f32) win_h);
    camera_fov(camera, 45.);
    camera_limits(camera, .1, 1000.);
    camera_position(camera, (struct vector3) { 0, 1, 2 });
    camera_target(camera, VECTOR3_ORIGIN);
}

/**
 * @brief
 *
 * @param shader
 */
void lisilisk_create_default_material_shader(
        struct shader *shader)
{
    if (!shader) {
        return;
    }

    shader_material_frag_mem(shader, default_fragment_start,
            (size_t) &default_fragment_size);
    shader_material_vert_mem(shader, default_vertex_start,
            (size_t) &default_vertex_size);

    shader_link(shader);
}
