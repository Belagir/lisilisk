/**
 * @file lisilisk_static.c
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <sys/time.h>

#include <lisilisk.h>
#include <ustd/res.h>

#include <3dful.h>
#include <resourceful.h>

#include "lisilisk_internals.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

DECLARE_RES(skybox_vert, "res_shaders_environment_skybox_vert_glsl")
DECLARE_RES(skybox_frag, "res_shaders_environment_skybox_frag_glsl")

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static struct model *static_data_model_of_instance(
        union lisk_handle_layout handle);

static struct model *static_data_model_of(
        lisk_res_t res_model);

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

    // TODO: refactor that like the .context
    struct {
        struct scene scene;
        struct camera camera;
        struct environment environment;
    } world;

    // TODO: make this held by the shader store.
    struct shader sky_shader;

    struct {
        struct lisilisk_store_texture textures;
        struct lisilisk_store_geometry geometries;
        struct lisilisk_store_material materials;
        struct lisilisk_store_shader shaders;
        struct lisilisk_store_model models;
    } stores;
} static_data;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief This function orchestrates the setup of the engine. It will set the
 * engine data into a blank state, ready to receive models and modifications.
 * Once this is done, the window can be shown with lisk_show().
 *
 * @param[in] name Name displayed on the window.
 * @param[in] resources_folder System path to a folder that will hold all of the engine's resources.
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

    static_data.stores.textures = lisilisk_store_texture_create();
    static_data.stores.geometries = lisilisk_store_geometry_create();
    static_data.stores.materials = lisilisk_store_material_create( &static_data.stores.textures);
    static_data.stores.shaders = lisilisk_store_shader_create();

    static_data.stores.models = lisilisk_store_model_create( &static_data.stores.materials,
            &static_data.stores.shaders);

    lisilisk_setup_camera(&static_data.world.camera, &static_data.context);

    shader_vert_mem(&static_data.sky_shader, skybox_vert_start, (size_t)&skybox_vert_size);
    shader_frag_mem(&static_data.sky_shader, skybox_frag_start, (size_t)&skybox_frag_size);
    shader_link(&static_data.sky_shader);

    lisilisk_setup_environment(&static_data.world.environment, static_data.stores.geometries.sphere,
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

    lisilisk_store_model_delete(&static_data.stores.models);
    lisilisk_store_geometry_delete(&static_data.stores.geometries);
    lisilisk_store_material_delete(&static_data.stores.materials);
    lisilisk_store_texture_delete(&static_data.stores.textures);
    lisilisk_store_shader_delete(&static_data.stores.shaders);

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
 * @brief Requests the size, in pixels, of the window.
 *
 * @param[out] width Pointer to an integer to receive the width.
 * @param[out] height Pointer to an integer to receive the height.
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
 * @return lisk_res_t
 */
lisk_res_t lisk_model(
        const char *name)
{
    union lisk_res_layout handle = { .full = LISK_RES_NONE };
    u32 hash = 0;

    hash = lisilisk_store_model_register(&static_data.stores.models, name);

    handle = (union lisk_res_layout) {
        .flavor = RES_REPRESENTS_MODEL,
        .hash = hash,
    };

    return handle.full;
}

/**
 * @brief Loads or retrieve a previously loaded texture. The texture can be
 * used with the handle that is returned.
 *
 * @param[in] file System path to an image inside your resources folder.
 * @return lisk_res_t
 */
lisk_res_t lisk_texture(
        const char *file)
{
    union lisk_res_layout handle = { .full = LISK_RES_NONE };
    u32 hash = 0;

    hash = lisilisk_store_texture_register(&static_data.stores.textures,
        static_data.context.res_manager, file);

    handle = (union lisk_res_layout) {
        .flavor = RES_REPRESENTS_TEXTURE,
        .hash = hash,
    };

    return handle.full;
}

/**
 * @brief
 *
 * @param frag_shader
 * @param vert_shader
 * @return lisk_res_t
 */
lisk_res_t lisk_shader(
        const char *frag_shader,
        const char *vert_shader)
{
    union lisk_res_layout handle = { .full = LISK_RES_NONE };
    u32 hash = 0;

    hash = lisilisk_store_shader_register(&static_data.stores.shaders,
        static_data.context.res_manager, frag_shader, vert_shader);

    handle = (union lisk_res_layout) {
        .flavor = RES_REPRESENTS_SHADER,
        .hash = hash
    };

    return handle.full;
}

/**
 * @brief
 *
 * @param name
 * @return lisk_res_t
 */
lisk_res_t lisk_material(
        const char *name)
{
    union lisk_res_layout handle = { .full = LISK_RES_NONE };
    u32 hash = 0;

    hash = lisilisk_store_material_register(&static_data.stores.materials,
            name);

    handle = (union lisk_res_layout) {
        .flavor = RES_REPRESENTS_MATERIAL,
        .hash = hash
    };

    return handle.full;
}

/**
 * @brief
 *
 * @param obj_file
 * @return lisk_res_t
 */
lisk_res_t lisk_geometry(
        const char *obj_file)
{
    union lisk_res_layout handle = { .full = LISK_RES_NONE };
    u32 hash = 0;

    hash = lisilisk_store_geometry_register(&static_data.stores.geometries,
            static_data.context.res_manager, obj_file);

    handle = (union lisk_res_layout) {
        .flavor = RES_REPRESENTS_GEOMETRY,
        .hash = hash
    };

    return handle.full;
}

/**
 * @brief
 *
 * @param material
 * @param uniform_name
 * @param value
 */
void lisk_material_set_uniform_float(
        lisk_res_t res_material,
        const char *uniform_name,
        float value)
{
    union lisk_res_layout handle = { .full = res_material };
    struct material* material = nullptr;

    if (handle.flavor != RES_REPRESENTS_MATERIAL) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle.hash);

    if (!material) {
        return;
    }

    if (!material_has_uniform(material, uniform_name)) {
        material_add_uniform_float(material, uniform_name, 1);
    }

    material_set_uniform(material, uniform_name, &value);
}

/**
 * @brief
 *
 * @param material
 * @param uniform_name
 * @param texture
 */
void lisk_material_set_uniform_texture(
        lisk_res_t res_material,
        const char *uniform_name,
        lisk_res_t res_texture)
{
    union lisk_res_layout handle_material = { .full = res_material };
    struct material* material = nullptr;
    union lisk_res_layout handle_texture = { .full = res_texture };
    struct texture* texture = nullptr;

    if (handle_material.flavor != RES_REPRESENTS_MATERIAL) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    if (!material) {
        return;
    }

    texture = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_texture.hash);

    if (!texture) {
        return;
    }

    if (!material_has_uniform(material, uniform_name)) {
        material_add_uniform_texture(material, uniform_name);
    }

    material_set_uniform(material, uniform_name, &texture);
}

/**
 * @brief Registers a model to the engine. Instances of this model will be able
 * to be rendered to the scene from this point on.
 *
 * The model starts with a default shader and material. The geometry is required
 * to have a valid model.
 *
 * @param[in] name New name to reference the model.
 */
void lisk_model_geometry(
        lisk_res_t res_model,
        lisk_res_t res_geometry)
{
    union lisk_res_layout geometry_handle = { .full = res_geometry };
    struct geometry *geometry = nullptr;
    struct model *model = nullptr;

    if (geometry_handle.flavor != RES_REPRESENTS_GEOMETRY) {
        return;
    }

    model = static_data_model_of(res_model);
    if (!model) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_store_geometry_retrieve(
            &static_data.stores.geometries, geometry_handle.hash);

    if (!geometry) {
        return;
    }

    model_geometry(model, geometry);
    scene_model(&static_data.world.scene, model);
}

/**
 * @brief
 *
 * @param[in] name Name used to reference this shader.
 */
void lisk_model_shader(
        lisk_res_t res_model,
        lisk_res_t res_shader)
{
    union lisk_res_layout shader_handle = { .full = res_shader };
    struct shader *shader = nullptr;
    struct model *model = nullptr;

    if (shader_handle.flavor != RES_REPRESENTS_SHADER) {
        return;
    }

    model = static_data_model_of(res_model);
    if (!model) {
        return;
    }

    shader = lisilisk_store_shader_retrieve(
            &static_data.stores.shaders, shader_handle.hash);

    if (!shader) {
        return;
    }

    model_shader(model, shader);
}

/**
 * @brief
 *
 * @param name
 * @param material
 */
void lisk_model_material(
        lisk_res_t res_model,
        lisk_res_t res_material)
{
    union lisk_res_layout material_handle = { .full = res_material };
    struct material *material = nullptr;
    struct model *model = nullptr;

    if (material_handle.flavor != RES_REPRESENTS_MATERIAL) {
        return;
    }

    model = static_data_model_of(res_model);
    if (!model) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, material_handle.hash);
    if (!material) {
        return;
    }

    model_material(model, material);
}

/**
 * @brief
 *
 * @param res_geometry
 * @param conf
 */
void lisk_geometry_configure(
        lisk_res_t res_geometry,
        enum lisk_geometry_conf conf)
{

    struct geometry *geometry = nullptr;

    union lisk_res_layout geometry_handle = { .full = res_geometry };

    if (geometry_handle.flavor != RES_REPRESENTS_GEOMETRY) {
        return;
    }

    // Create the geometry from the file and registers it in a map
    geometry = lisilisk_store_geometry_retrieve(
            &static_data.stores.geometries, geometry_handle.hash);

    if (!geometry) {
        return;
    }

    switch (conf) {
        case LISK_GEOMETRY_CULL_FRONT:
            geometry_set_culling(geometry, GEOMETRY_CULL_FRONT);
            break;
        case LISK_GEOMETRY_CULL_BACK:
            geometry_set_culling(geometry, GEOMETRY_CULL_BACK);
            break;
        case LISK_GEOMETRY_CULL_NONE:
            geometry_set_culling(geometry, GEOMETRY_CULL_NONE);
            break;
        case LISK_GEOMETRY_IN_BACK:
            geometry_set_layering(geometry, GEOMETRY_LAYER_BACK);
            break;
        case LISK_GEOMETRY_IN_FRONT:
            geometry_set_layering(geometry, GEOMETRY_LAYER_FRONT);
            break;
        case LISK_GEOMETRY_IN_SCENE:
            geometry_set_layering(geometry, GEOMETRY_LAYER_NORMAL);
            break;
    }
}

/**
 * @brief
 *
 * @param material
 * @param texture
 */
void lisk_material_base_texture(
        lisk_res_t res_material,
        lisk_res_t res_texture)
{
    union lisk_res_layout handle_material = { .full = res_material };
    union lisk_res_layout handle_texture = { .full = res_texture };
    struct material *material = nullptr;
    struct texture *texture = nullptr;

    if ((handle_material.flavor != RES_REPRESENTS_MATERIAL)
            || (handle_texture.flavor != RES_REPRESENTS_TEXTURE)) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    texture = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_texture.hash);

    if (material && texture) {
        material_texture(material, texture);
    }
}

/**
 * @brief
 *
 * @param material
 * @param ambient
 * @param texture_mask
 */
void lisk_material_ambient(
        lisk_res_t res_material,
        float (*ambient)[4],
        lisk_res_t res_texture_mask)
{
    union lisk_res_layout handle_material = { .full = res_material };
    union lisk_res_layout handle_mask = { .full = res_texture_mask };
    struct material *material = nullptr;
    struct texture *mask = nullptr;

    if ((handle_material.flavor != RES_REPRESENTS_MATERIAL)
            || (handle_mask.flavor != RES_REPRESENTS_TEXTURE)) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    mask = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_mask.hash);

    if (material) {
        if (ambient) {
            material_ambient(material, *ambient, (*ambient)[3]);
        }
        if (mask) {
            material_ambient_mask(material, mask);
        }
    }
}

/**
 * @brief
 *
 * @param material
 * @param diffuse
 * @param texture_mask
 */
void lisk_material_diffuse(
        lisk_res_t res_material,
        float (*diffuse)[4],
        lisk_res_t res_texture_mask)
{
    union lisk_res_layout handle_material = { .full = res_material };
    union lisk_res_layout handle_mask = { .full = res_texture_mask };
    struct material *material = nullptr;
    struct texture *mask = nullptr;

    if ((handle_material.flavor != RES_REPRESENTS_MATERIAL)
            || (handle_mask.flavor != RES_REPRESENTS_TEXTURE)) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    mask = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_mask.hash);

    if (material) {
        if (diffuse) {
            material_diffuse(material, *diffuse, (*diffuse)[3]);
        }
        if (mask) {
            material_diffuse_mask(material, mask);
        }
    }
}

// Sets how a model reflects the light sources.
void lisk_material_specular(
        lisk_res_t res_material,
        float (*specular)[4],
        float shininess,
        lisk_res_t res_texture_mask)
{
    union lisk_res_layout handle_material = { .full = res_material };
    union lisk_res_layout handle_mask = { .full = res_texture_mask };
    struct material *material = nullptr;
    struct texture *mask = nullptr;

    if ((handle_material.flavor != RES_REPRESENTS_MATERIAL)
            || (handle_mask.flavor != RES_REPRESENTS_TEXTURE)) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    mask = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_mask.hash);

    if (material) {
        material_shininess(material, shininess);

        if (specular) {
            material_specular(material, *specular, (*specular)[3]);
        }
        if (mask) {
            material_specular_mask(material, mask);
        }
    }
}

/**
 * @brief
 *
 * @param material
 * @param emission
 * @param texture_mask
 */
void lisk_material_emission(
        lisk_res_t res_material,
        float (*emission)[4],
        lisk_res_t res_texture_mask)
{
    union lisk_res_layout handle_material = { .full = res_material };
    union lisk_res_layout handle_mask = { .full = res_texture_mask };
    struct material *material = nullptr;
    struct texture *mask = nullptr;

    if ((handle_material.flavor != RES_REPRESENTS_MATERIAL)
            || (handle_mask.flavor != RES_REPRESENTS_TEXTURE)) {
        return;
    }

    material = lisilisk_store_material_retrieve(
            &static_data.stores.materials, handle_material.hash);

    mask = lisilisk_store_texture_retrieve(
            &static_data.stores.textures, handle_mask.hash);

    if (material) {
        if (emission) {
            material_emissive(material, *emission, (*emission)[3]);
        }
        if (mask) {
            material_emissive_mask(material, mask);
        }
    }
}

/**
 * @brief Creates an instance of a model at some point in space.
 * The function returns a handle referencing the new instance within the model.
 *
 * @param[in] name String containing the name of a previously registered model.
 * @param[in] pos 3D position of the new instance.
 * @param[in] scale scale of the model.
 * @return lisk_handle_t
 */
lisk_handle_t lisk_model_instanciate(
        lisk_res_t res_model,
        float (*pos)[3],
        float scale)
{
    struct model *model = nullptr;
    union lisk_handle_layout handle = { .full = 0 };
    union lisk_res_layout model_handle = { .full = res_model };
    handle_t in_handle = 0;

    model = static_data_model_of(res_model);
    if (!model) {
        return LISK_HANDLE_NONE;
    }

    model_instantiate(model, &in_handle);
    model_instance_position(model, in_handle,
            (struct vector3) { (*pos)[0], (*pos)[1], (*pos)[2] });
    model_instance_scale(model, in_handle,
            (f32[3]) { scale, scale, scale });
    model_instance_rotation(model, in_handle,
            quaternion_identity());

    handle = (union lisk_handle_layout) {
            .hash = model_handle.hash,
            .flavor = HANDLE_REPRESENTS_INSTANCE,
            .internal = in_handle
    };

    return handle.full;
}

/**
 * @brief Adds a directional light to the scene, a source of light that
 * simulates a bright, but far away object. All of its rays are parallel
 * to each other.
 *
 * @param[in] direction Light rays direction.
 * @param[in] color Light color.
 * @return lisk_handle_t
 */
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
 * @brief Adds a point light to the scene, a source of light that is limited
 * sin space. All of its rays take the same origin and its strength fade with
 * distance.
 *
 * @param[in] direction Light position.
 * @param[in] color Light color.
 * @param[in] constant Constant attenuation (doesn't vary with distance).
 * @param[in] linear Linear attenuation (gets stronger with distance).
 * @param[in] quadratic Quadratic attenuation (gets very strong with distance).
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
 * @brief Returns a handle to the scene camera.
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
 * @brief Deletes an instance of a model or a light from the engine.
 *
 * @param[in] instance Handle to the deleted object.
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
 * @brief Changes the scale of a model's instance.
 *
 * @param[in] instance Handle to an instance.
 * @param[in] scale New scale.
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
 * @brief Changes the position of an intance, a point light, or the camera.
 *
 * @param[in] instance Handle to the repositioned object.
 * @param[in] pos New position.
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
 * @brief Changes the rotation of an intance, a directional light, or the
 * camera.
 *
 * @param[in] instance Handle to the reoriented object.
 * @param[in] axis Axis of rotation.
 * @param[in] angle_rad Angle, in radians, of the object around the axis.
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
 * @brief Changes the rotation of an intance, a directional light, or the
 * camera, using a quaternion.
 *
 * @param[in] instance Handle to the reoriented object.
 * @param[in] q Quaternion encoding the rotation.
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
            camera_up(&static_data.world.camera,
                    vector3_rotate_by_quaternion(VECTOR3_Y_POSITIVE,
                            *(struct quaternion *) q));
            return;
    }
}

/**
 * @brief Changes the attenuation factors of a point light.
 *
 * @param[in] instance handle to a point light.
 * @param[in] constant Constant attenuation (doesn't vary with distance).
 * @param[in] linear Linear attenuation (gets stronger with distance).
 * @param[in] quadratic Quadratic attenuation (gets very strong with distance).
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
 * @brief Changes the field of view of the camera.
 *
 * @param[in] instance Handle to the camera.
 * @param[in] fov New field of view angle, in degrees.
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
 * @brief Changes the distances to the near and far planes of the camera,
 * from which stops and start clipping.
 *
 * @param[in] instance Handle to the camera.
 * @param[in] near Distance to the near plane.
 * @param[in] far Distance to the far plane.
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
 * @brief Changes the camera's target point.
 *
 * @param[in] instance Handle to the camera.
 * @param[in] point Point the camera nows looks at.
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
 * @brief Changes the environment's background to a cubemap texture.
 * The order of images is : Right, Left, Top, Bottom, Front, Back.
 *
 * @param[in] cubemap Set of six paths to iamges.
 */
void lisk_skybox_set(
        const char *(*cubemap)[6])
{
    struct texture *texture = nullptr;

    if (!static_data.active) {
        return;
    }

    texture = lisilisk_store_texture_cubemap_cache(
            &static_data.stores.textures,
            static_data.context.res_manager, cubemap);
    environment_skybox(&static_data.world.environment, texture);
}

/**
 * @brief Sets the color of the background, in absence of a cubemap texture.
 *
 * @param[in] color New color.
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
 * @brief Updates the scene once. This will render all models that are set to be
 * shown, and swap the window. This call will try to synchronize itself with the
 * refresh rate of the monitor.
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
 * @brief Hides the window. This will actually unload the scene from the GPU.
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
 * @brief retrieves the model that generated an instance, from the handle.
 *
 * @param instance_handle
 * @return struct model*
 */
static struct model *static_data_model_of_instance(
        union lisk_handle_layout handle)
{
    struct model *model = nullptr;

    if (!static_data.active || (handle.flavor != HANDLE_REPRESENTS_INSTANCE)) {
        return nullptr;
    }

    model = lisilisk_store_model_retrieve(&static_data.stores.models,
            handle.hash);

    return model;
}

/**
 * @brief retrieves a model from the handle the user supplied.
 *
 * @param res_model
 * @return struct model*
 */
static struct model *static_data_model_of(
        lisk_res_t res_model)
{
    union lisk_res_layout model_handle = { .full = res_model };
    struct model *model = nullptr;

    if (!static_data.active || (model_handle.flavor != RES_REPRESENTS_MODEL)) {
        return nullptr;
    }

    model = lisilisk_store_model_retrieve(
            &static_data.stores.models, model_handle.hash);

    return model;
}
