
#include "3dful_collections.h"

#include <ustd/array.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void scene_lights_bind_uniform_blocks(struct scene *scene, struct model *model);
static void scene_lights_send_uniforms(struct scene *scene, struct model *model);
static void scene_time_send_uniforms(u32 time, struct model *model);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param scene
 */
void scene_create(struct scene *scene)
{
    *scene = (struct scene) {
            .load_state = { .flags = LOADABLE_FLAG_NONE, .nb_users = 0u },

            .models_array = array_create(make_system_allocator(), sizeof(*(scene->models_array)), 256),

            .camera = { .view = matrix4_identity(), .projection = matrix4_identity() },

            .point_lights_array = array_create(make_system_allocator(), sizeof(*scene->point_lights_array), 32),
            .direc_lights_array = array_create(make_system_allocator(), sizeof(*scene->direc_lights_array), 8),
    };
}

/**
 * @brief
 *
 * @param scene
 */
void scene_delete(struct scene *scene)
{
    array_destroy(make_system_allocator(), (void **) &scene->models_array);
    array_destroy(make_system_allocator(), (void **) &scene->point_lights_array);
    array_destroy(make_system_allocator(), (void **) &scene->direc_lights_array);
}

/**
 * @brief
 *
 * @param scene
 * @param model
 */
void scene_model(struct scene *scene, struct model *model)
{
    if (scene->load_state.flags & LOADABLE_FLAG_LOADED) {
        model_load(model);
        scene_lights_bind_uniform_blocks(scene, model);
    }
    array_push(scene->models_array, &model);
}

/**
 * @brief
 *
 * @param scene
 * @param camera
 */
void scene_camera(struct scene *scene, struct camera camera)
{
    scene->camera = camera;
}

/**
 * @brief
 *
 * @param scene
 * @param light
 */
void scene_light_point(struct scene *scene, struct light_point light)
{
    array_push(scene->point_lights_array, &light);
}

/**
 * @brief
 *
 * @param scene
 * @param light
 */
void scene_light_direc(struct scene *scene, struct light_directional light)
{
    array_push(scene->direc_lights_array, &light);
}

/**
 * @brief
 *
 * @param scene
 * @param light
 */
void scene_light_ambient(struct scene *scene, struct light light)
{
    scene->ambient_light = light;
}

/**
 * @brief
 *
 * @param scene
 */
void scene_draw(struct scene scene, u32 time)
{
    for (size_t i = 0 ; i < array_length(scene.models_array) ; i++) {
        scene_lights_send_uniforms(&scene, scene.models_array[i]);
        camera_send_uniforms(&scene.camera, scene.models_array[i]);
        scene_time_send_uniforms(time, scene.models_array[i]);

        model_draw(*(scene.models_array[i]));
    }
}

/**
 * @brief
 *
 * @param scene
 */
void scene_load(struct scene *scene)
{
    loadable_add_user((struct loadable *) scene);

    if (loadable_needs_loading((struct loadable *) scene)) {
        // Load lights -- point lights
        glGenBuffers(1, &scene->ubo_point_lights);
        glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_point_lights);
        glBufferData(GL_UNIFORM_BUFFER,
                array_length(scene->point_lights_array) * sizeof(*scene->point_lights_array),
                scene->point_lights_array, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Load lights -- point lights
        glGenBuffers(1, &scene->ubo_dir_lights);
        glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_dir_lights);
        glBufferData(GL_UNIFORM_BUFFER,
                array_length(scene->direc_lights_array) * sizeof(*scene->direc_lights_array),
                scene->direc_lights_array, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        scene->load_state.flags |= LOADABLE_FLAG_LOADED;

        // load models assigned to the scene
        for (size_t i = 0 ; i < array_length(scene->models_array) ; i++) {
            model_load(scene->models_array[i]);
            scene_lights_bind_uniform_blocks(scene, scene->models_array[i]);
        }
    }
}

/**
 * @brief
 *
 * @param scene
 */
void scene_unload(struct scene *scene)
{
    loadable_remove_user((struct loadable *) scene);

    if (loadable_needs_unloading((struct loadable *) scene)) {
        // destroy buffers
        glDeleteBuffers(1, &scene->ubo_point_lights);
        glDeleteBuffers(1, &scene->ubo_dir_lights);

        scene->load_state.flags &= ~LOADABLE_FLAG_LOADED;

        for (size_t i = 0 ; i < array_length(scene->models_array) ; i++) {
            model_unload(scene->models_array[i]);
        }
    }

}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param scene
 * @param model
 */
static void scene_lights_bind_uniform_blocks(struct scene *scene, struct model *model)
{
    GLint block_name = -1;

    glUseProgram(model->shader->program);
    {
        block_name = glGetUniformBlockIndex(model->shader->program, "BLOCK_LIGHT_POINTS");
        glUniformBlockBinding(model->shader->program, block_name, SHADER_UBO_LIGHT_POINT);

        glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_point_lights);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_name, scene->ubo_point_lights);

        block_name = glGetUniformBlockIndex(model->shader->program, "BLOCK_LIGHT_DIRECTIONALS");
        glUniformBlockBinding(model->shader->program, block_name, SHADER_UBO_LIGHT_DIREC);

        glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_dir_lights);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_name, scene->ubo_dir_lights);
    }
    glUseProgram(0);
}

/**
 * @brief
 *
 * @param scene
 * @param model
 */
static void scene_lights_send_uniforms(struct scene *scene, struct model *model)
{
    GLint uniform_name = -1;

    glUseProgram(model->shader->program);
    {
        uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_POINTS_NB");
        glUniform1ui(uniform_name, array_length(scene->point_lights_array));

        uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_DIRECTIONALS_NB");
        glUniform1ui(uniform_name, array_length(scene->direc_lights_array));

        uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_AMBIENT");
        glUniform4fv(uniform_name, 1, scene->ambient_light.color);
    }
    glUseProgram(0);
}


/**
 * @brief
 *
 * @param time
 * @param model
 */
static void scene_time_send_uniforms(u32 time, struct model *model)
{
    GLint uniform_name = -1;

    glUseProgram(model->shader->program);
    {
        uniform_name = glGetUniformLocation(model->shader->program, "TIME");
            glUniform1ui(uniform_name, time);
    }
    glUseProgram(0);
}
