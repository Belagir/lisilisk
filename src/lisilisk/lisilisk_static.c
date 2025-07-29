
#include <lisilisk.h>

#include "../3dful/3dful.h"
#include "lisilisk_internals.h"
#include "stores/lisilisk_stores.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
static struct {
    bool active;

    struct application app;
    struct scene scene;

    struct camera camera;
    struct environment environment;

    struct texture empty_texture;
    struct shader default_shader;
    struct material default_material;

    struct lisilisk_geometry_store geometry_store;
} lisilisk_static;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
void lisk_init(void)
{
    if (lisilisk_static.active) {
        return;
    }

    lisilisk_static.app = application_create("Lisilisk", 1200, 800);

    lisilisk_default_camera(&lisilisk_static.camera,
            lisilisk_static.app.sdl_window);

    lisilisk_default_environment(&lisilisk_static.environment);

    scene_create(&lisilisk_static.scene);
    scene_camera(&lisilisk_static.scene, &lisilisk_static.camera);
    scene_environment(&lisilisk_static.scene, &lisilisk_static.environment);

    lisilisk_create_default_texture(&lisilisk_static.empty_texture);
    lisilisk_create_default_material_shader(&lisilisk_static.default_shader);
    lisilisk_default_material(&lisilisk_static.default_material,
            &lisilisk_static.empty_texture);

    lisilisk_static.geometry_store = lisilisk_geometry_store_create();

    lisilisk_static.active = true;
}

/**
 * @brief
 *
 */
void lisk_deinit(void)
{
    if (!lisilisk_static.active) {
        return;
    }

    lisilisk_geometry_store_delete(&lisilisk_static.geometry_store);

    texture_delete(&lisilisk_static.empty_texture);
    shader_delete(&lisilisk_static.default_shader);

    scene_delete(&lisilisk_static.scene);
    application_destroy(&lisilisk_static.app);

    lisilisk_static.active = false;
}

/**
 * @brief
 *
 * @param width
 * @param height
 */
void lisk_resize(uint16_t width, uint16_t height)
{
    if (!lisilisk_static.active) {
        return;
    }

    SDL_SetWindowSize(lisilisk_static.app.sdl_window, width, height);
}

/**
 * @brief
 *
 * @param window_name
 */
void lisk_rename(const char *window_name)
{
    if (!lisilisk_static.active) {
        return;
    }

    SDL_SetWindowTitle(lisilisk_static.app.sdl_window, window_name);
}

/**
 * @brief
 *
 * @param name
 * @param obj_file
 */
void lisk_model(
        const char *name,
        const char *obj_file)
{
    struct geometry *geometry = nullptr;

    if (!lisilisk_static.active) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_geometry_store_item(&lisilisk_static.geometry_store,
            obj_file);

    if (!geometry) {
        return;
    }

    // Create the model, register it in a map


    // Give the model a default shader

    // Give the model a default material
}

/**
 * @brief
 *
 */
void lisk_show(void)
{
    SDL_Event event = { };

    if (!lisilisk_static.active) {
        return;
    }

    SDL_ShowWindow(lisilisk_static.app.sdl_window);

    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
        }

        scene_draw(&lisilisk_static.scene, 0);

        SDL_GL_SwapWindow(lisilisk_static.app.sdl_window);

        SDL_Delay(100);
    }
}
