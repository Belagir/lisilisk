
#include <sys/time.h>
#include <GLES3/gl3.h>

#include <lisilisk.h>
#include <ustd/res.h>

#include <3dful.h>
#include <resourceful.h>

#include "lisilisk_internals.h"

DECLARE_RES(skybox_vert, "res_shaders_environment_skybox_vert_glsl")
DECLARE_RES(skybox_frag, "res_shaders_environment_skybox_frag_glsl")

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static struct model * static_data_model_of_instance(
        union lisk_handle_layout handle);

static struct model * static_data_model_named(
        const char *name,
        u32 *out_hash);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief This data chunk contains the global, unique lisilisk engine internals
 * used to simplify the library usage.
 */
static struct {
    bool active;
    struct logger *log;

    struct lisilisk_context context;

    struct {
        struct scene scene;
        struct camera camera;
        struct environment environment;
    } world;

    struct shader sky_shader;

    struct {
        struct lisilisk_store_texture texture_store;
        struct lisilisk_store_geometry geometry_store;
        struct lisilisk_store_material material_store;
        struct lisilisk_store_shader shader_store;
        struct lisilisk_store_model model_store;
    } stores;
} static_data;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief This function orchestrates the setup of the engine. It will set the
 * engine data into a blank state, ready to receive models and modifications.
 * Once this is done, the window can be shown with lisk_show().
 */
void lisk_init(const char *name, const char *resources_folder)
{
    if (static_data.active) {
        return;
    }

    static_data.log = logger_create(stderr, LOGGER_ON_DESTROY_DO_NOTHING);

    if (!resources_folder) {
        logger_log(static_data.log, LOGGER_SEVERITY_ERRO,
                "No resource folder given. You will not be able to interact with the file system !\n");
        return;
    }

    lisilisk_context_init(
            &static_data.context,
            static_data.log,
            name, 1200, 800);

    lisilisk_context_integrate_resources(&static_data.context,
            resources_folder);

    static_data.stores.texture_store = lisilisk_store_texture_create();
    static_data.stores.geometry_store = lisilisk_store_geometry_create();
    static_data.stores.material_store = lisilisk_store_material_create(
            &static_data.stores.texture_store);
    static_data.stores.shader_store = lisilisk_store_shader_create();

    static_data.stores.model_store = lisilisk_store_model_create(
            &static_data.stores.material_store,
            &static_data.stores.shader_store);

    lisilisk_setup_camera(&static_data.world.camera, &static_data.context);

    shader_vert_mem(&static_data.sky_shader, skybox_vert_start,
            (size_t)&skybox_vert_size);
    shader_frag_mem(&static_data.sky_shader, skybox_frag_start,
            (size_t)&skybox_frag_size);
    shader_link(&static_data.sky_shader);

    lisilisk_setup_environment(&static_data.world.environment,
            static_data.stores.geometry_store.sphere,
            &static_data.sky_shader);

    scene_create(&static_data.world.scene);
    scene_camera(&static_data.world.scene, &static_data.world.camera);
    scene_environment(&static_data.world.scene, &static_data.world.environment);

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

    shader_delete(&static_data.sky_shader);

    lisilisk_store_model_delete(&static_data.stores.model_store);
    lisilisk_store_geometry_delete(&static_data.stores.geometry_store);
    lisilisk_store_material_delete(&static_data.stores.material_store);
    lisilisk_store_texture_delete(&static_data.stores.texture_store);

    scene_delete(&static_data.world.scene);

    lisilisk_context_deinit(&static_data.context);

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

    lisilisk_context_window_set_size(&static_data.context, width, height);
}

/**
 * @brief
 *
 * @param width
 * @param height
 */
void lisk_get_size(
        int32_t *width,
        int32_t *height)
{
    if (!static_data.active) {
        return;
    }

    lisilisk_context_window_get_size(&static_data.context, width, height);
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

    lisilisk_context_window_set_name(&static_data.context, window_name);
}

/**
 * @brief
 *
 * @param name
 */
void lisk_model_show(
        const char *name)
{
    struct model *model = nullptr;

    model = static_data_model_named(name, nullptr);
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

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_store_geometry_cache(&static_data.stores.geometry_store,
            static_data.context.res_manager,
            obj_file);

    if (!geometry) {
        return;
    }

    model_geometry(model, geometry);
}

/**
 * @brief
 *
 * @param name
 * @param frag_shader
 * @param vert_shader
 */
void lisk_model_material_shader(
        const char *name,
        const char *frag_shader,
        const char *vert_shader)
{
    struct model *model = nullptr;
    struct shader *shader = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    shader = lisilisk_store_shader_material_cache(
            &static_data.stores.shader_store,
            static_data.context.res_manager, frag_shader, vert_shader);

    if (!shader) {
        return;
    }

    model_shader(model, shader);
}

/**
 * @brief
 *
 * @param name
 * @param frag_shader
 * @param vert_shader
 */
void lisk_model_advanced_shader(
        const char *name,
        const char *frag_shader,
        const char *vert_shader)
{
    struct model *model = nullptr;
    struct shader *shader = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    shader = lisilisk_store_shader_cache(
            &static_data.stores.shader_store,
            static_data.context.res_manager, frag_shader, vert_shader);

    if (!shader) {
        return;
    }

    model_shader(model, shader);
}

/**
 * @brief
 *
 * @param name
 */
void lisk_model_frontface_culling(
        const char *name)
{
    struct model *model = nullptr;
    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    geometry_set_culling(model->geometry, GEOMETRY_CULL_FRONT);
}

/**
 * @brief
 *
 * @param name
 */
void lisk_model_backface_culling(
        const char *name)
{
    struct model *model = nullptr;
    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    geometry_set_culling(model->geometry, GEOMETRY_CULL_BACK);
}

/**
 * @brief
 *
 * @param name
 */
void lisk_model_noface_culling(
        const char *name)
{
    struct model *model = nullptr;
    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    geometry_set_culling(model->geometry, GEOMETRY_CULL_NONE);
}

/**
 * @brief
 *
 */
void lisk_model_draw_in_back(
        const char *name)
{
    struct model *model = nullptr;
    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    geometry_set_layering(model->geometry, GEOMETRY_LAYER_BACK);
}

/**
 * @brief
 *
 * @param name
 */
void lisk_model_draw_in_front(
        const char *name)
{
    struct model *model = nullptr;
    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    geometry_set_layering(model->geometry, GEOMETRY_LAYER_FRONT);
}

/**
 * @brief
 *
 * @param name
 * @param texture
 */
void lisk_model_base_texture(
        const char *name,
        const char *texture)
{
    struct model *model = nullptr;
    struct texture *base = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    base = lisilisk_store_texture_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, texture);
    material_texture(model->material, base);
}

/**
 * @brief
 *
 * @param name
 * @param ambient
 */
void lisk_model_ambient_color(
        const char *name,
        float (*ambient)[4])
{
    struct model *model = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    material_ambient(model->material, *ambient, (*ambient)[3]);
}

/**
 * @brief
 *
 * @param name
 * @param texture_mask
 */
void lisk_model_ambient_mask(
        const char *name,
        const char *texture_mask)
{
    struct model *model = nullptr;
    struct texture *mask = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    mask = lisilisk_store_texture_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, texture_mask);
    material_ambient_mask(model->material, mask);
}

/**
 * @brief
 *
 * @param name
 * @param diffuse
 */
void lisk_model_diffuse_color(
        const char *name,
        float (*diffuse)[4])
{
    struct model *model = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    material_diffuse(model->material, *diffuse, (*diffuse)[3]);
}
/**
 * @brief
 *
 * @param name
 * @param texture_mask
 */
void lisk_model_diffuse_mask(
        const char *name,
        const char *texture_mask)
{
    struct model *model = nullptr;
    struct texture *mask = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    mask = lisilisk_store_texture_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, texture_mask);
    material_diffuse_mask(model->material, mask);
}

/**
 * @brief
 *
 * @param name
 * @param specular
 */
void lisk_model_specular_color(
        const char *name,
        float (*specular)[4],
        float shininess)
{
    struct model *model = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    material_specular(model->material, *specular, (*specular)[3]);
    material_shininess(model->material, shininess);
}

/**
 * @brief
 *
 * @param name
 * @param texture_mask
 */
void lisk_model_specular_mask(
        const char *name,
        const char *texture_mask)
{
    struct model *model = nullptr;
    struct texture *mask = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    mask = lisilisk_store_texture_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, texture_mask);
    material_specular_mask(model->material, mask);
}

/**
 * @brief
 *
 * @param name
 * @param emission
 */
void lisk_model_emission_color(
        const char *name,
        float (*emission)[4])
{
    struct model *model = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    material_emissive(model->material, *emission, (*emission)[3]);
}

/**
 * @brief
 *
 * @param name
 * @param texture_mask
 */
void lisk_model_emission_mask(
        const char *name,
        const char *texture_mask)
{
    struct model *model = nullptr;
    struct texture *mask = nullptr;

    model = static_data_model_named(name, nullptr);
    if (!model) {
        return;
    }

    mask = lisilisk_store_texture_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, texture_mask);
    material_emissive_mask(model->material, mask);
}

/**
 * @brief Creates an instance of a model at some point in space.
 * The function returns a handle referencing the new instance within the model.
 *
 * @param[in] name String containing the name of a previously registered
 * model.
 * @param[in] pos 3D position of the new instance.
 * @param[in] scale scale of the model.
 * @return lisk_handle_t
 */
lisk_handle_t lisk_model_instanciate(
        const char *name,
        float (*pos)[3],
        float scale)
{
    struct model *model = nullptr;
    u32 model_hash = 0;
    union lisk_handle_layout handle = { .full = 0 };
    handle_t in_handle = 0;

    model = static_data_model_named(name, &model_hash);
    if (!model) {
        return LISK_HANDLE_NONE;
    }

    model_instantiate(model, &in_handle);
    model_instance_position(model, in_handle,
        (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
    model_instance_scale(model, in_handle, (f32[3]) { scale, scale, scale });
    model_instance_rotation(model, in_handle,
        quaternion_identity());

    handle = (union lisk_handle_layout) {
            .hash = model_hash,
            .flavor = HANDLE_REPRESENTS_INSTANCE,
            .internal = in_handle
    };

    return handle.full;
}

lisk_handle_t lisk_directional_light_add(
        float (*direction)[3],
        float (*color)[4])
{
    union lisk_handle_layout handle = { .full = 0 };
    handle_t in_handle = 0;

    if (!static_data.active) {
        return LISK_HANDLE_NONE;
    }

    scene_light_direc(&static_data.world.scene, &in_handle);
    scene_light_direc_color(&static_data.world.scene, in_handle, *color);
    scene_light_direc_orientation(&static_data.world.scene, in_handle,
            (struct vector3) {
                (*direction)[0],
                (*direction)[1],
                (*direction)[2] });

    handle = (union lisk_handle_layout) {
            .hash = 0,
            .flavor = HANDLE_REPRESENTS_LIGHT_DIREC,
            .internal = in_handle
    };

    return handle.full;
}

/**
 * @brief
 *
 * @param position
 * @param color
 * @return lisk_handle_t
 */
lisk_handle_t lisk_point_light_add(
        float (*position)[3],
        float (*color)[4],
        float constant,
        float linear,
        float quadratic)
{
    union lisk_handle_layout handle = { .full = 0 };
    handle_t in_handle = 0;

    if (!static_data.active) {
        return LISK_HANDLE_NONE;
    }

    scene_light_point(&static_data.world.scene, &in_handle);
    scene_light_point_color(&static_data.world.scene, in_handle, *color);
    scene_light_point_position(&static_data.world.scene, in_handle,
            (struct vector3) {
                (*position)[0],
                (*position)[1],
                (*position)[2] });
    scene_light_point_attenuation(&static_data.world.scene, in_handle,
            constant, linear ,quadratic);

    handle = (union lisk_handle_layout) {
            .hash = 0,
            .flavor = HANDLE_REPRESENTS_LIGHT_POINT,
            .internal = in_handle
    };

    return handle.full;
}

/**
 * @brief
 *
 * @return lisk_handle_t
 */
lisk_handle_t lisk_camera(void)
{
    union lisk_handle_layout handle = { .full = 0 };

    if (!static_data.active) {
        return LISK_HANDLE_NONE;
    }

    handle = (union lisk_handle_layout) {
            .hash = 0,
            .flavor = HANDLE_REPRESENTS_CAMERA,
            .internal = 0
    };

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
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            model_instance_remove(
                    static_data_model_of_instance(handle), handle.internal);
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            scene_light_direc_remove(&static_data.world.scene, handle.internal);
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            scene_light_point_remove(&static_data.world.scene, handle.internal);
            return;
        case HANDLE_REPRESENTS_CAMERA:
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param scale
 */

void lisk_instance_set_scale(
        lisk_handle_t instance,
        float (*scale)[3])
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            model_instance_scale(
                    static_data_model_of_instance(handle),
                    handle.internal, *scale);
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            return;
        case HANDLE_REPRESENTS_CAMERA:
            return;
    }
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
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            model_instance_position(static_data_model_of_instance(handle),
                    handle.internal,
                    (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            scene_light_point_position(&static_data.world.scene,
                    handle.internal,
                    (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
            return;
        case HANDLE_REPRESENTS_CAMERA:
            camera_position(&static_data.world.camera,
                    (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param pos
 */
void lisk_instance_set_rotation(
        lisk_handle_t instance,
        float (*axis)[3],
        float angle_rad)
{
    struct quaternion q = quaternion_from_axis_and_angle(
            (struct vector3) { (*axis)[0], (*axis)[1], (*axis)[2] },
            angle_rad);

    lisk_instance_set_rotation_quaternion(instance, (float (*)[4]) &q);
}

/**
* @brief
*
* @param instance
* @param q
*/
void lisk_instance_set_rotation_quaternion(
        lisk_handle_t instance,
        float (*q)[4])
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            model_instance_rotation(static_data_model_of_instance(handle),
                    handle.internal, *(struct quaternion *) q);
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            scene_light_direc_orientation(&static_data.world.scene,
                    handle.internal,
                    vector3_rotate_by_quaternion(VECTOR3_Y_POSITIVE,
                            *(struct quaternion *) q));
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            return;
        case HANDLE_REPRESENTS_CAMERA:
            camera_target(&static_data.world.camera,
                    vector3_add(static_data.world.camera.pos,
                    vector3_rotate_by_quaternion(VECTOR3_Z_NEGATIVE,
                            *(struct quaternion *) q)));
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param constant
 * @param linear
 * @param quadratic
 */
void lisk_instance_light_point_set_attenuation(
        lisk_handle_t instance,
        float constant, float linear, float quadratic)
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            scene_light_point_attenuation(&static_data.world.scene,
                    handle.internal,
                    constant, linear, quadratic);
            return;
        case HANDLE_REPRESENTS_CAMERA:
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param fov
 */
void lisk_instance_camera_set_fov(
        lisk_handle_t instance,
        float fov)
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            return;
        case HANDLE_REPRESENTS_CAMERA:
            camera_fov(&static_data.world.camera, fov);
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param near
 * @param far
 */
void lisk_instance_camera_set_limits(
        lisk_handle_t instance,
        float near, float far)
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            return;
        case HANDLE_REPRESENTS_CAMERA:
            camera_limits(&static_data.world.camera, near, far);
            return;
    }
}

/**
 * @brief
 *
 * @param instance
 * @param point
 */
void lisk_instance_camera_set_target(
        lisk_handle_t instance,
        float (*point)[3])
{
    union lisk_handle_layout handle = { .full = instance };

    switch ((enum handle_flavor) handle.flavor) {
        case HANDLE_IS_INVALID:
            return;
        case HANDLE_REPRESENTS_INSTANCE:
            return;
        case HANDLE_REPRESENTS_LIGHT_DIREC:
            return;
        case HANDLE_REPRESENTS_LIGHT_POINT:
            return;
        case HANDLE_REPRESENTS_CAMERA:
            camera_target(&static_data.world.camera,
                    (struct vector3) { (*point)[0], (*point)[1], (*point)[2] });
            return;
    }
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
 * @brief
 *
 * @param cubemap
 */
void lisk_skybox_set(
        const char *(*cubemap)[6])
{
    struct texture *texture = nullptr;

    if (!static_data.active) {
        return;
    }

    texture = lisilisk_store_texture_cubemap_cache(
            &static_data.stores.texture_store,
            static_data.context.res_manager, cubemap);
    environment_skybox(&static_data.world.environment, texture);
}

/**
 * @brief
 *
 * @param color
 */
void lisk_bg_color_set(
        float (*color)[3])
{
    if (!static_data.active) {
        return;
    }

    environment_bg(&static_data.world.environment, *color);
}

/**
 * @brief Loads the scene, and shows the window that renders the OpenGL context.
 * The function will block until the user quits the window.
 */
void lisk_show(void)
{
    if (!static_data.active) {
        return;
    }

    scene_load(&static_data.world.scene);
    SDL_ShowWindow(static_data.context.window);
}

/**
 * @brief
 *
 */
void lisk_draw(void)
{
    static struct timeval last_call = { 0 };

    struct timeval this_call = { 0 };
    f32 seconds_elapsed = 0.;

    if (!static_data.active) {
        return;
    }

    gettimeofday(&this_call, NULL);
    seconds_elapsed = (this_call.tv_sec - last_call.tv_sec)
                        + ((this_call.tv_usec - last_call.tv_usec) / 1000000.);

    scene_draw(&static_data.world.scene, seconds_elapsed);
    SDL_GL_SwapWindow(static_data.context.window);

    last_call = this_call;
}

/**
 * @brief
 *
 */
void lisk_hide(void)
{
    if (!static_data.active) {
        return;
    }

    scene_unload(&static_data.world.scene);
    SDL_HideWindow(static_data.context.window);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param instance_handle
 * @return struct model*
 */
static struct model * static_data_model_of_instance(
        union lisk_handle_layout handle)
{
    struct model *model = nullptr;

    if (!static_data.active) {
        return nullptr;
    }

    if (handle.flavor != HANDLE_REPRESENTS_INSTANCE) {
        return nullptr;
    }

    model = lisilisk_store_model_retrieve(&static_data.stores.model_store,
            handle.hash);

    return model;
}

/**
 * @brief
 *
 * @param name
 * @return struct model*
 */
static struct model * static_data_model_named(
        const char *name,
        u32 *out_hash)
{
    struct model *model = nullptr;
    u32 model_hash = 0;

    if (!static_data.active) {
        return nullptr;
    }

    // Create the model, register it in a map
    model_hash = lisilisk_store_model_register(
            &static_data.stores.model_store, name);
    model = lisilisk_store_model_retrieve(
            &static_data.stores.model_store, model_hash);

    if (out_hash) {
        *out_hash = model_hash;
    }

    return model;
}
