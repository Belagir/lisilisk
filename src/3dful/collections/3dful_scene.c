
#include "3dful_collections.h"

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void scene_lights_bind_uniform_blocks(struct scene *scene, struct shader *shader);
static void scene_lights_send_uniforms(struct scene *scene, struct shader *shader);
static void scene_time_send_uniforms(u32 time, struct shader *shader);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

            .camera = nullptr,

            .light_sources = {
                .point_lights_array = array_create(make_system_allocator(),
                        sizeof(*scene->light_sources.point_lights_array), 32),
                .point_lights = { { 0 }, 0 },

                .direc_lights_array = array_create(make_system_allocator(),     
                        sizeof(*scene->light_sources.direc_lights_array), 8),
                .direc_lights = { { 0 }, 0 },
            }
    };

    handle_buffer_array_create(&scene->light_sources.point_lights);
    handle_buffer_array_bind(&scene->light_sources.point_lights, 
            scene->light_sources.point_lights_array);
    scene->light_sources.point_lights.buffer_usage = GL_UNIFORM_BUFFER;

    handle_buffer_array_create(&scene->light_sources.direc_lights);
    handle_buffer_array_bind(&scene->light_sources.direc_lights, 
            scene->light_sources.direc_lights_array);
    scene->light_sources.direc_lights.buffer_usage = GL_UNIFORM_BUFFER;
}

/**
 * @brief
 *
 * @param scene
 */
void scene_delete(struct scene *scene)
{
    handle_buffer_array_delete(&scene->light_sources.point_lights);
    handle_buffer_array_delete(&scene->light_sources.direc_lights);

    array_destroy(make_system_allocator(), (void **) &scene->models_array);
    array_destroy(make_system_allocator(), (void **) &scene->light_sources.point_lights_array);
    array_destroy(make_system_allocator(), (void **) &scene->light_sources.direc_lights_array);

    *scene = (struct scene) { 0 };
}

/**
 * @brief
 *
 * @param scene
 * @param model
 */
void scene_model(struct scene *scene, struct model *model)
{
    if (!model) {
        return;
    }

    if (scene->load_state.flags & LOADABLE_FLAG_LOADED) {
        model_load(model);
    }
    array_push(scene->models_array, &model);
}

/**
 * @brief
 *
 * @param scene
 * @param camera
 */
void scene_camera(struct scene *scene, struct camera *camera)
{
    scene->camera = camera;
}

/**
 * @brief
 *
 * @param scene
 * @param env
 */
void scene_environment(struct scene *scene, struct environment *env)
{
    if (scene->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (scene->env) {
            environment_unload(scene->env);
        }
        if (env) {
            environment_load(env);
        }
    }

    scene->env = env;
}

/**
 * @brief 
 * 
 * @param scene 
 * @param out_handle 
 */
void scene_light_point(struct scene *scene, handle_t *out_handle)
{
    handle_buffer_array_push(&scene->light_sources.point_lights, out_handle);
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 * @param pos 
 */
void scene_light_point_position(struct scene *scene, handle_t handle, struct vector3 pos)
{
    handle_buffer_array_set(&scene->light_sources.point_lights, handle, 
            &pos, OFFSET_OF(struct light_point, position), sizeof(pos));
}

/**
 * @brief 
 * 
 */
void scene_light_point_color(struct scene *scene, handle_t handle, f32 color[4])
{
    handle_buffer_array_set(&scene->light_sources.point_lights, handle, 
            color, OFFSET_OF(struct light_point, color), sizeof(f32[4]));
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 * @param constant 
 * @param linear 
 * @param quadratic 
 */
void scene_light_point_attenuation(struct scene *scene, handle_t handle, f32 constant, f32 linear, f32 quadratic)
{
    handle_buffer_array_set(&scene->light_sources.point_lights, handle, 
            &constant, OFFSET_OF(struct light_point, constant), 
            sizeof(constant));
    handle_buffer_array_set(&scene->light_sources.point_lights, handle, 
            &linear, OFFSET_OF(struct light_point, linear), 
            sizeof(linear));
    handle_buffer_array_set(&scene->light_sources.point_lights, handle, 
            &quadratic, OFFSET_OF(struct light_point, quadratic), 
            sizeof(quadratic));
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 */
void scene_light_point_remove(struct scene *scene, handle_t handle)
{
    handle_buffer_array_remove(&scene->light_sources.point_lights, handle);
}

/**
 * @brief 
 * 
 * @param scene 
 * @param out_handle 
 */
void scene_light_direc(struct scene *scene, handle_t *out_handle)
{
    handle_buffer_array_push(&scene->light_sources.direc_lights, out_handle);
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 * @param dir 
 */
void scene_light_direc_orientation(struct scene *scene, handle_t handle, struct vector3 dir)
{
    handle_buffer_array_set(&scene->light_sources.direc_lights, handle, 
            &dir, OFFSET_OF(struct light_directional, direction), sizeof(dir));
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 * @param color 
 */
void scene_light_direc_color(struct scene *scene, handle_t handle, f32 color[4])
{
    handle_buffer_array_set(&scene->light_sources.direc_lights, handle, 
            color, OFFSET_OF(struct light_directional, color), sizeof(f32[4]));
}

/**
 * @brief 
 * 
 * @param scene 
 * @param handle 
 */
void scene_light_direc_remove(struct scene *scene, handle_t handle)
{
    handle_buffer_array_remove(&scene->light_sources.direc_lights, handle);
}

/**
 * @brief
 *
 * @param scene
 */
void scene_draw(struct scene *scene, u32 time)
{
    if (scene->env) {
        glClearColor(scene->env->bg_color[0], scene->env->bg_color[1], scene->env->bg_color[2], 1.);
    } else {
        glClearColor(.1, .1, .1, 1.);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        if (scene->env) {
            camera_send_uniforms(scene->camera, scene->env->shader);
            environment_draw(scene->env);
        }

        for (size_t i = 0 ; i < array_length(scene->models_array) ; i++) {
            if (scene->env) environment_send_uniforms(scene->env, scene->models_array[i]->shader);
            if (scene->camera) camera_send_uniforms(scene->camera, scene->models_array[i]->shader);

            scene_lights_bind_uniform_blocks(scene, 
                    scene->models_array[i]->shader);
            scene_lights_send_uniforms(scene, scene->models_array[i]->shader);

            scene_time_send_uniforms(time, scene->models_array[i]->shader);

            model_draw(scene->models_array[i]);
        }
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
        handle_buffer_array_load(&scene->light_sources.point_lights);
        // Load lights -- directional lights
        handle_buffer_array_load(&scene->light_sources.direc_lights);

        scene->load_state.flags |= LOADABLE_FLAG_LOADED;

        // load models assigned to the scene
        for (size_t i = 0 ; i < array_length(scene->models_array) ; i++) {
            model_load(scene->models_array[i]);
        }

        if (scene->env) {
            environment_load(scene->env);
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
        
        scene->load_state.flags &= ~LOADABLE_FLAG_LOADED;
        handle_buffer_array_unload(&scene->light_sources.point_lights);
        handle_buffer_array_unload(&scene->light_sources.direc_lights);

        for (size_t i = 0 ; i < array_length(scene->models_array) ; i++) {
            model_unload(scene->models_array[i]);
        }

        if (scene->env) {
            environment_unload(scene->env);
        }
    }

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param scene
 * @param model
 */
static void scene_lights_bind_uniform_blocks(struct scene *scene, struct shader *shader)
{
    GLint block_name = -1;

    glUseProgram(shader->program);
    {
        block_name = glGetUniformBlockIndex(shader->program, "BLOCK_LIGHT_POINTS");
        glUniformBlockBinding(shader->program, block_name, SHADER_UBO_LIGHT_POINT);

        glBindBuffer(GL_UNIFORM_BUFFER,
                scene->light_sources.point_lights.buffer_name);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_name, 
                scene->light_sources.point_lights.buffer_name);

        block_name = glGetUniformBlockIndex(shader->program, "BLOCK_LIGHT_DIRECTIONALS");
        glUniformBlockBinding(shader->program, block_name, SHADER_UBO_LIGHT_DIREC);

        glBindBuffer(GL_UNIFORM_BUFFER, 
                scene->light_sources.direc_lights.buffer_name);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_name, 
                scene->light_sources.direc_lights.buffer_name);
    }
    glUseProgram(0);
}

/**
 * @brief
 *
 * @param scene
 * @param model
 */
static void scene_lights_send_uniforms(struct scene *scene, struct shader *shader)
{
    GLint uniform_name = -1;

    glUseProgram(shader->program);
    {
        uniform_name = glGetUniformLocation(shader->program, "LIGHT_POINTS_NB");
        glUniform1ui(uniform_name, array_length(scene->light_sources.point_lights_array));

        uniform_name = glGetUniformLocation(shader->program, "LIGHT_DIRECTIONALS_NB");
        glUniform1ui(uniform_name, array_length(scene->light_sources.direc_lights_array));
    }
    glUseProgram(0);
}

/**
 * @brief
 *
 * @param time
 * @param model
 */
static void scene_time_send_uniforms(u32 time, struct shader *shader)
{
    GLint uniform_name = -1;

    glUseProgram(shader->program);
    {
        uniform_name = glGetUniformLocation(shader->program, "TIME");
            glUniform1ui(uniform_name, time);
    }
    glUseProgram(0);
}
