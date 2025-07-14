
#include "3dful_collections.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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
            .models = range_create_dynamic(make_system_allocator(), sizeof(*scene->models->data), 256),
            .camera = { .view = matrix4_identity(), .projection = matrix4_identity() },
            .point_lights = range_create_dynamic(make_system_allocator(), sizeof(*scene->point_lights->data), 32),
            .direc_lights = range_create_dynamic(make_system_allocator(), sizeof(*scene->direc_lights->data), 8),
    };
}

/**
 * @brief
 *
 * @param scene
 */
void scene_delete(struct scene *scene)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(scene->models));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(scene->point_lights));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(scene->direc_lights));
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
    }
    range_push(RANGE_TO_ANY(scene->models), &model);
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
    range_push(RANGE_TO_ANY(scene->point_lights), &light);
}

/**
 * @brief
 *
 * @param scene
 * @param light
 */
void scene_light_direc(struct scene *scene, struct light_directional light)
{
    range_push(RANGE_TO_ANY(scene->direc_lights), &light);
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
    for (size_t i = 0 ; i < scene.models->length ; i++) {
        camera_send_uniforms(&scene.camera, scene.models->data[i]);
        scene_time_send_uniforms(time, scene.models->data[i]);
        model_draw(*(scene.models->data[i]));
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
                scene->point_lights->length * sizeof(*scene->point_lights->data),
                scene->point_lights->data, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Load lights -- point lights
        glGenBuffers(1, &scene->ubo_dir_lights);
        glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_dir_lights);
        glBufferData(GL_UNIFORM_BUFFER,
                scene->direc_lights->length * sizeof(*scene->direc_lights->data),
                scene->direc_lights->data, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        scene->load_state.flags |= LOADABLE_FLAG_LOADED;

        // load models assigned to the scene
        for (size_t i = 0 ; i < scene->models->length ; i++) {
            scene_lights_send_uniforms(scene, scene->models->data[i]);
            model_load(scene->models->data[i]);
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

        for (size_t i = 0 ; i < scene->models->length ; i++) {
            model_unload(scene->models->data[i]);
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
static void scene_lights_send_uniforms(struct scene *scene, struct model *model)
{
    (void) scene;

    GLint uniform_name = -1;
    GLint block_name = -1;

    glUseProgram(model->shader->program);
    {
        glBindVertexArray(model->gpu_side.vao);
        {
            block_name = glGetUniformBlockIndex(model->shader->program, "BLOCK_LIGHT_POINTS");
            glUniformBlockBinding(model->shader->program, block_name, SHADER_UBO_LIGHT_POINT);

            glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_point_lights);
            glBindBufferBase(GL_UNIFORM_BUFFER, block_name, scene->ubo_point_lights);

            block_name = glGetUniformBlockIndex(model->shader->program, "BLOCK_LIGHT_DIRECTIONALS");
            glUniformBlockBinding(model->shader->program, block_name, SHADER_UBO_LIGHT_DIREC);

            glBindBuffer(GL_UNIFORM_BUFFER, scene->ubo_dir_lights);
            glBindBufferBase(GL_UNIFORM_BUFFER, block_name, scene->ubo_dir_lights);

            // -----

            uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_POINTS_NB");
            glUniform1ui(uniform_name, scene->point_lights->length);

            uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_DIRECTIONALS_NB");
            glUniform1ui(uniform_name, scene->direc_lights->length);

            uniform_name = glGetUniformLocation(model->shader->program, "LIGHT_AMBIENT");
            glUniform4fv(uniform_name, 1, scene->ambient_light.color);
        }
        glBindVertexArray(0);
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
        glBindVertexArray(model->gpu_side.vao);
        {
            uniform_name = glGetUniformLocation(model->shader->program, "TIME");
            glUniform1ui(uniform_name, time);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
