/**
 * @file 3dful_model.c
 * @author Gabriel Bédat
 * @brief Implementation of model-related procedures.
 * @version 0.1
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Allocates memory for a model object.
 *
 * @param[out] model Allocated model object.
 */
void model_create(struct model *model)
{
    *model = (struct model) {
            .load_state = { 0 },

            .shader = nullptr,
            .geometry = nullptr,
            .material = nullptr,

            .instances_array = array_create(make_system_allocator(),
                    sizeof(*model->instances_array), 32),
            .instances = { { 0 }, 0 },

            .gpu_side = { 0 },
    };

    handle_buffer_array_create(&model->instances);
    handle_buffer_array_bind(&model->instances, model->instances_array);
    model->instances.buffer_usage = GL_ARRAY_BUFFER;
}

/**
 * @brief Releases memory held by a model object.
 *
 * @param[inout] model Destroyed model object.
 */
void model_delete(struct model *model)
{
    array_destroy(make_system_allocator(), (void **) &model->instances_array);
    handle_buffer_array_delete(&model->instances);

    *model = (struct model) { 0 };
}

/**
 * @brief Links a model to a geometry. The model will be rendered with the
 * geometry's mesh.
 *
 * @param[inout] model Modified model.
 * @param[in] geometry New 3D data for this model.
 */
void model_geometry(struct model *model, struct geometry *geometry)
{
    if (model->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (model->geometry) {
            geometry_unload(model->geometry);
        }
        if (geometry) {
            geometry_load(geometry);
        }
    }

    model->geometry = geometry;
}

/**
 * @brief Links a model to a shader. The model's geometry will be rendered with
 * this shader.
 *
 * @param[inout] model Modified model.
 * @param[in] shader New shader for this model.
 */
void model_shader(struct model *model, struct shader *shader)
{
    model->shader = shader;
}

/**
 * @brief Links a model to a material. The model's shader will receive the
 * material's data.
 *
 * @param[inout] model Modified model.
 * @param[in] material New material for this model.
 */
void model_material(struct model *model, struct material *material)
{
    if (model->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (model->material) {
            material_unload(model->material);
        }
        if (material) {
            material_load(material);
        }
    }

    model->material = material;
}

/**
 * @brief Adds an instance of this model to be rendered in the world space.
 *
 * @param[inout] model Model to instanciate.
 * @param[out] out_handle Pointer to a handle, filled with the id of the new
 * instance.
 */
void model_instantiate(struct model *model, handle_t *out_handle)
{
    handle_buffer_array_push(&model->instances, out_handle);
}

/**
 * @brief Sets the position of some instance of a model.
 *
 * @param[inout] model Model the instance belongs to.
 * @param[in] handle Handle to an instance of this model.
 * @param[in] pos New position assigned to this instance.
 */
void model_instance_position(struct model *model, handle_t handle,
        struct vector3 pos)
{
    handle_buffer_array_set(&model->instances, handle,
            &pos, OFFSET_OF(struct instance, position),
            sizeof(pos));
}

/**
 * @brief Sets the rotation of some instance of a model.
 *
 * @param[inout] model Model the instance belongs to.
 * @param[in] handle Handle to an instance of this model.
 * @param[in] rotation New quaternion rotation assigned to this instance.
 */
void model_instance_rotation(struct model *model, handle_t handle,
        struct quaternion rotation)
{
    handle_buffer_array_set(&model->instances, handle,
            &rotation, OFFSET_OF(struct instance, rotation),
            sizeof(rotation));
}

/**
 * @brief Sets the uniform scale of some instance of a model.
 *
 * @param[inout] model Model the instance belongs to.
 * @param[in] handle Handle to an instance of this model.
 * @param[in] pos New scale factor assigned to this instance.
 */
void model_instance_scale(struct model *model, handle_t handle,
        f32 scale)
{
    handle_buffer_array_set(&model->instances, handle,
            &scale, OFFSET_OF(struct instance, scale),
            sizeof(scale));
}

/**
 * @brief Removes an instance from a model. The handle becomes unusable.
 *
 * @param[inout] model Model the instance belongs to.
 * @param[inout] handle Handle to an instance of this model.
 */
void model_instance_remove(struct model *model, handle_t handle)
{
    handle_buffer_array_remove(&model->instances, handle);
}

/**
 * @brief Loads a model to the GPU with OpenGL.
 *
 * @param[inout] model Loaded model.
 */
void model_load(struct model *model)
{
    loadable_add_user((struct loadable *) model);

    if (loadable_needs_loading((struct loadable *) model)) {

        if (model->geometry) geometry_load(model->geometry);
        if (model->material) material_load(model->material);

        handle_buffer_array_load(&model->instances);

        // model's vao
        glGenVertexArrays(1, &model->gpu_side.vao);

        glUseProgram(model->shader->program);
        // binding scenario for this VAO
        glBindVertexArray(model->gpu_side.vao);

        // vertex data from geometry
        if (model->geometry) {
            glBindBuffer(GL_ARRAY_BUFFER,
                    model->geometry->gpu_side.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                    model->geometry->gpu_side.ebo);
        }

        glVertexAttribPointer(SHADER_VERT_POS, 3, GL_FLOAT, GL_FALSE,
                sizeof(struct vertex),
                (void *) OFFSET_OF(struct vertex, pos));
        glEnableVertexAttribArray(SHADER_VERT_POS);

        glVertexAttribPointer(SHADER_VERT_NORMAL, 3, GL_FLOAT,
                GL_FALSE, sizeof(struct vertex),
                (void *) OFFSET_OF(struct vertex, normal));
        glEnableVertexAttribArray(SHADER_VERT_NORMAL);

        glVertexAttribPointer(SHADER_VERT_UV, 2, GL_FLOAT, GL_FALSE,
                sizeof(struct vertex),
                (void *) OFFSET_OF(struct vertex, uv));
        glEnableVertexAttribArray(SHADER_VERT_UV);

        // instances data
        glBindBuffer(GL_ARRAY_BUFFER, model->instances.buffer_name);

        glEnableVertexAttribArray(SHADER_VERT_INSTANCEPOSITION);
        glVertexAttribPointer(SHADER_VERT_INSTANCEPOSITION, 3,
                GL_FLOAT, GL_FALSE, sizeof(struct instance),
                (void*) (OFFSET_OF(struct instance, position)));

        glEnableVertexAttribArray(SHADER_VERT_INSTANCESCALE);
        glVertexAttribPointer(SHADER_VERT_INSTANCESCALE, 1,
                GL_FLOAT, GL_FALSE, sizeof(struct instance),
                (void*) (OFFSET_OF(struct instance, scale)));

        glEnableVertexAttribArray(SHADER_VERT_INSTANCEROTATION);
        glVertexAttribPointer(SHADER_VERT_INSTANCEROTATION, 4,
                GL_FLOAT, GL_FALSE, sizeof(struct instance),
                (void*) (OFFSET_OF(struct instance, rotation)));

        glVertexAttribDivisor(SHADER_VERT_INSTANCEPOSITION, 1);
        glVertexAttribDivisor(SHADER_VERT_INSTANCESCALE, 1);
        glVertexAttribDivisor(SHADER_VERT_INSTANCEROTATION, 1);

        glBindVertexArray(0);
        glUseProgram(0);

        model->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief Unloads the model from GPU memory.
 *
 * @param[inout] model Unloaded model.
 */
void model_unload(struct model *model)
{
    loadable_remove_user((struct loadable *) model);

    if (loadable_needs_unloading((struct loadable *) model)) {

        glDeleteVertexArrays(1, &model->gpu_side.vao);
        model->gpu_side.vao = 0;

        model->load_state.flags &= ~LOADABLE_FLAG_LOADED;

        if (model->geometry) geometry_unload(model->geometry);
        if (model->material) material_unload(model->material);

        handle_buffer_array_unload(&model->instances);
    }
}

/**
 * @brief Renders a model's instances to the current OpenGL context.
 * The model should have been loaded.
 *
 * @param[in] model Drawn model.
 */
void model_draw(struct model *model)
{
    if (model->material) {
        material_bind_uniform_blocks(model->material, model->shader);
        material_bind_textures(model->material, model->shader);
    }

    glUseProgram(model->shader->program);
    glBindVertexArray(model->gpu_side.vao);
    if (model->geometry) {
        glDrawElementsInstanced(GL_TRIANGLES,
                array_length(model->geometry->faces_array) * 3,
                GL_UNSIGNED_INT, 0,
                array_length(model->instances_array));
    }
    glBindVertexArray(0);
    glUseProgram(0);
}
