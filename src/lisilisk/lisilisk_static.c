
#include <lisilisk.h>

#include "../3dful/3dful.h"
#include "lisilisk_internals.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static struct model * static_data_model_of(union lisk_handle_layout handle);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief This data chunk contains the global, unique lisilisk engine internals
 * used to simplify the library usage.
 */
static struct {
    bool active;

    struct application app;
    struct {
        struct scene scene;
        struct camera camera;
        struct environment environment;
    } world;

    struct {
        struct lisilisk_geometry_store geometry_store;
        struct lisilisk_model_store model_store;
    } stores;
} static_data;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief This function orchestrates the setup of the engine. It will set the
 * engine data into a blank state, ready to receive models and modifications.
 * Once this is done, the window can be shown with lisk_show().
 */
void lisk_init(void)
{
    if (static_data.active) {
        return;
    }

    static_data.app = application_create("Lisilisk", 1200, 800);

    lisilisk_default_camera(&static_data.world.camera,
            static_data.app.sdl_window);

    lisilisk_default_environment(&static_data.world.environment);

    scene_create(&static_data.world.scene);
    scene_camera(&static_data.world.scene, &static_data.world.camera);
    scene_environment(&static_data.world.scene, &static_data.world.environment);

    static_data.stores.geometry_store = lisilisk_geometry_store_create();
    static_data.stores.model_store = lisilisk_model_store_create();

    static_data.active = true;
}

/**
 * @brief Wipes the engine data. This will release all memory, taken by the
 * models, shaders, etc...
 */
void lisk_deinit(void)
{
    if (!static_data.active) {
        return;
    }

    lisilisk_geometry_store_delete(&static_data.stores.geometry_store);
    lisilisk_model_store_delete(&static_data.stores.model_store);

    scene_delete(&static_data.world.scene);
    application_destroy(&static_data.app);

    static_data.active = false;
}

/**
 * @brief Changes the dimensions of the window showing the OpenGL context.
 *
 * @param[in] width Width, in pixels, of the window.
 * @param[in] height Height, in pixels, of the window.
 */
void lisk_resize(uint16_t width, uint16_t height)
{
    if (!static_data.active) {
        return;
    }

    SDL_SetWindowSize(static_data.app.sdl_window, width, height);
}

/**
 * @brief Changes the title of the window.
 *
 * @param[in] window_name New name for the window.
 */
void lisk_rename(const char *window_name)
{
    if (!static_data.active) {
        return;
    }

    SDL_SetWindowTitle(static_data.app.sdl_window, window_name);
}

void lisk_model_show(
        const char *name)
{
    struct model *model = nullptr;
    u32 model_hash = 0;

    model_hash = lisilisk_model_store_item(
            &static_data.stores.model_store, name);
    model = lisilisk_model_store_retrieve(
            &static_data.stores.model_store, model_hash);

    if (!model) {
        return;
    }

    scene_model(&static_data.world.scene, model);

}

/**
 * @brief Registers a model to the engine. Instances of this model will be able
 * to be rendered to the scene from this point on.
 *
 * The model starts with a default shader and material. The geometry is required
 * to have a valid model.
 *
 * @param[in] name New name to reference the model.
 * @param[in] obj_file Object file from which the model takes its geometry from.
 */
void lisk_model_geometry(
        const char *name,
        const char *obj_file)
{
    struct geometry *geometry = nullptr;
    struct model *model = nullptr;
    u32 model_hash = 0;

    if (!static_data.active) {
        return;
    }

    // Create the model, register it in a map
    model_hash = lisilisk_model_store_item(
            &static_data.stores.model_store, name);
    model = lisilisk_model_store_retrieve(
            &static_data.stores.model_store, model_hash);

    if (!model) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_geometry_store_item(&static_data.stores.geometry_store,
            obj_file);

    if (!geometry) {
        return;
    }

    model_geometry(model, geometry);
}

/**
 * @brief Creates an instance of a model at some point in space.
 * The function returns a handle referencing the new instance within the model.
 *
 * @param[in] model_name String containing the name of a previously registered
 * model.
 * @param[in] pos 3D position of the new instance.
 * @return lisk_handle_t
 */
lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3])
{
    struct model *model = nullptr;
    u32 name_hash = 0;
    union lisk_handle_layout handle = { .full = 0 };

    if (!static_data.active) {
        return LISK_HANDLE_NONE;
    }

    if (!model_name || !pos) {
        return LISK_HANDLE_NONE;
    }

    name_hash = lisilisk_model_store_item(
            &static_data.stores.model_store, model_name);
    model = lisilisk_model_store_retrieve(&static_data.stores.model_store,
            name_hash);

    if (!model) {
        return LISK_HANDLE_NONE;
    }

    handle.hash = name_hash;

    model_instantiate(model, &handle.internal_handle);
    model_instance_position(model, handle.internal_handle,
            (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
    model_instance_scale(model, handle.internal_handle, 1.);
    model_instance_rotation(model, handle.internal_handle, quaternion_identity());

    return handle.full;
}

/**
 * @brief Deletes an instance from a model registered to the engine.
 *
 * @param[in] instance Handle to the deleted instance.
 */
void lisk_instance_remove(
        lisk_handle_t instance)
{
    struct model *model = nullptr;
    union lisk_handle_layout handle = { .full = instance };

    model = static_data_model_of(handle);

    if (!model) {
        return;
    }

    model_instance_remove(model, handle.internal_handle);
}

/**
 * @brief
 *
 * @param instance
 * @param scale
 */
void lisk_instance_set_scale(
        lisk_handle_t instance,
        float scale)
{
    struct model *model = nullptr;
    union lisk_handle_layout handle = { .full = instance };

    model = static_data_model_of(handle);

    if (!model) {
        return;
    }

    model_instance_scale(model, handle.internal_handle, scale);
}

/**
 * @brief
 *
 * @param instance
 * @param pos
 */
void lisk_instance_set_position(
        lisk_handle_t instance,
        float (*pos)[3])
{
    struct model *model = nullptr;
    union lisk_handle_layout handle = { .full = instance };

    model = static_data_model_of(handle);

    if (!model) {
        return;
    }

    model_instance_position(model, handle.internal_handle,
            (struct vector3) { (*pos)[0], (*pos)[2], (*pos)[3] });
}

/**
 * @brief
 *
 * @param instance
 * @param pos
 */
void lisk_instance_rotate(
        lisk_handle_t instance,
        float (*axis)[3],
        float angle_rad)
{
    struct model *model = nullptr;
    union lisk_handle_layout handle = { .full = instance };
    struct vector3 axis_vec = { (*axis)[0], (*axis)[1], (*axis)[2] };

    model = static_data_model_of(handle);

    if (!model) {
        return;
    }

    model_instance_rotation(model, handle.internal_handle,
            quaternion_from_axis_and_angle(axis_vec, angle_rad));
}

/**
 * @brief Changes the ambient lighting of the scene.
 *
 * @param[in] r Red component, normalized.
 * @param[in] g Green component, normalized.
 * @param[in] b Blue component, normalized.
 * @param strength Strength modifier.
 */
void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength)
{
    if (!static_data.active) {
        return;
    }

    environment_ambient(&static_data.world.environment,
            (struct light) { .color = { r, g, b, strength } });
}

/**
 * @brief Loads the scene, and shows the window that renders the OpenGL context.
 * The function will block until the user quits the window.
 */
void lisk_show(void)
{
    SDL_Event event = { };

    if (!static_data.active) {
        return;
    }

    scene_load(&static_data.world.scene);
    SDL_ShowWindow(static_data.app.sdl_window);

    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
        }

        scene_draw(&static_data.world.scene, 0);

        SDL_GL_SwapWindow(static_data.app.sdl_window);

        SDL_Delay(100);
    }

    scene_unload(&static_data.world.scene);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param instance_handle
 * @return struct model*
 */
static struct model * static_data_model_of(union lisk_handle_layout handle)
{
    struct model *model = nullptr;

    if (!static_data.active) {
        return nullptr;
    }

    if (handle.internal_handle == LISK_HANDLE_NONE) {
        return nullptr;
    }

    model = lisilisk_model_store_retrieve(&static_data.stores.model_store,
            handle.hash);

    return model;
}

