
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

    struct texture blank_texture;
    struct shader default_shader;
    struct material default_material;

    struct lisilisk_geometry_store geometry_store;
    struct lisilisk_model_store model_store;
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

    lisilisk_create_default_texture(&lisilisk_static.blank_texture);
    lisilisk_create_default_material_shader(&lisilisk_static.default_shader);
    lisilisk_default_material(&lisilisk_static.default_material,
            &lisilisk_static.blank_texture);

    lisilisk_static.geometry_store = lisilisk_geometry_store_create();
    lisilisk_static.model_store = lisilisk_model_store_create();

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
    lisilisk_model_store_delete(&lisilisk_static.model_store);

    texture_delete(&lisilisk_static.blank_texture);
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
    struct model *model = nullptr;

    if (!lisilisk_static.active) {
        return;
    }

    // Create the model, register it in a map
    model = lisilisk_model_store_item(&lisilisk_static.model_store, name);

    if (!model) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_geometry_store_item(&lisilisk_static.geometry_store,
            obj_file);

    if (!geometry) {
        return;
    }

    model_geometry(model, geometry);
    model_shader(model, &lisilisk_static.default_shader);
    model_material(model, &lisilisk_static.default_material);

    scene_model(&lisilisk_static.scene, model);
}

/**
 * @brief
 *
 * @param model_name
 * @param pos
 * @param scale
 * @return lisk_handle_t
 */
lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3],
        float scale)
{
    struct model *model = nullptr;
    handle_t internal_handle = 0;

    if (!model_name || !pos) {
        return LISK_HANDLE_NONE;
    }

    model = lisilisk_model_store_retrieve(&lisilisk_static.model_store,
            model_name);

    if (!model) {
        return LISK_HANDLE_NONE;
    }

    model_instantiate(model, &internal_handle);
    model_instance_position(model, internal_handle,
            (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
    model_instance_scale(model, internal_handle, scale);
    model_instance_rotation(model, internal_handle, quaternion_identity());

    return (lisk_handle_t) internal_handle;
}

/**
 * @brief
 *
 * @param instance
 */
void lisk_model_instance_remove(
        const char *model_name,
        lisk_handle_t instance)
{
    struct model *model = nullptr;

    if (!model_name || (instance == LISK_HANDLE_NONE)) {
        return;
    }

    model = lisilisk_model_store_retrieve(&lisilisk_static.model_store,
            model_name);

    if (!model) {
        return;
    }

    model_instance_remove(model, (handle_t) instance);
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

    scene_load(&lisilisk_static.scene);
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
