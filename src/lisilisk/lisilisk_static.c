
#include <lisilisk.h>

#include "../3dful/3dful.h"

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
    scene_create(&lisilisk_static.scene);

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

        SDL_Delay(100);
    }
}
