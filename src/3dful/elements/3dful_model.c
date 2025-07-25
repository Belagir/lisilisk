
#include "3dful_core.h"

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
/**
 * @brief
 *
 * @param model
 */
void model_create(struct model *model)
{
    *model = (struct model) {
            .load_state = { 0 },

            .shader = nullptr,
            .geometry = nullptr,
            .material = nullptr,

            .instances_array = array_create(make_system_allocator(), sizeof(*model->instances_array), 32),
            .instances = { { 0 }, 0 },

            .gpu_side = { 0 },
    };

    handle_buffer_array_create(&model->instances);
    handle_buffer_array_bind(&model->instances, model->instances_array);
    model->instances.buffer_usage = GL_ARRAY_BUFFER;
}

/**
 * @brief
 *
 * @param model
 */
void model_delete(struct model *model)
{
    array_destroy(make_system_allocator(), (void **) &model->instances_array);
    handle_buffer_array_delete(&model->instances);

    *model = (struct model) { 0 };
}

/**
 * @brief Links a model to a geometry. The model will be rendered with the geometry's mesh.
 *
 * @param[inout] model
 * @param[in] geometry
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
 * @brief Links a model to a shader. The model's geometry will be rendered with this shader.
 *
 * @param[inout] model
 * @param[in] shader
 */
void model_shader(struct model *model, struct shader *shader)
{
    model->shader = shader;
}

/**
 * @brief Links a model to a material. The model's shader will receive the material's data.
 *
 * @param[inout] model
 * @param[in] material
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
 * @brief
 *
 * @param model
 * @param out_handle
 */
void model_instantiate(struct model *model, handle_t *out_handle)
{   
    handle_buffer_array_push(&model->instances, out_handle);
}

/**
 * @brief
 *
 * @param model
 * @param handle
 * @param tr
 */
void model_instance_transform(struct model *model, handle_t handle, struct matrix4 tr)
{
    handle_buffer_array_set(&model->instances, handle, &tr, 0, sizeof(tr));
}

/**
 * @brief
 *
 * @param model
 * @param handle
 */
void model_instance_remove(struct model *model, handle_t handle)
{
    handle_buffer_array_remove(&model->instances, handle);
}

/**
 * @brief Loads a model to the GPU with OpenGL.
 * Most of the trafic will be to tell which data is linked and sent to the shader.
 *
 * @param[inout] model
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
        {
            // binding scenario for this VAO
            glBindVertexArray(model->gpu_side.vao);
            {
                // vertex data from geometry
                if (model->geometry) {
                    glBindBuffer(GL_ARRAY_BUFFER, model->geometry->gpu_side.vbo);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->geometry->gpu_side.ebo);
                }

                glVertexAttribPointer(SHADER_VERT_POS, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
                glEnableVertexAttribArray(SHADER_VERT_POS);
                glVertexAttribPointer(SHADER_VERT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, normal));
                glEnableVertexAttribArray(SHADER_VERT_NORMAL);
                glVertexAttribPointer(SHADER_VERT_UV, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, uv));
                glEnableVertexAttribArray(SHADER_VERT_UV);

                // instances data
                glBindBuffer(GL_ARRAY_BUFFER, model->instances.buffer_name);
                glEnableVertexAttribArray(SHADER_VERT_INSTANCEMATRIX_ROW0);
                glVertexAttribPointer(SHADER_VERT_INSTANCEMATRIX_ROW0, 4, GL_FLOAT, GL_FALSE,
                        16*sizeof(float), (void*) (OFFSET_OF(struct matrix4, m0)));
                glEnableVertexAttribArray(SHADER_VERT_INSTANCEMATRIX_ROW1);
                glVertexAttribPointer(SHADER_VERT_INSTANCEMATRIX_ROW1, 4, GL_FLOAT, GL_FALSE,
                        16*sizeof(float), (void*) (OFFSET_OF(struct matrix4, m4)));
                glEnableVertexAttribArray(SHADER_VERT_INSTANCEMATRIX_ROW2);
                glVertexAttribPointer(SHADER_VERT_INSTANCEMATRIX_ROW2, 4, GL_FLOAT, GL_FALSE,
                        16*sizeof(float), (void*) (OFFSET_OF(struct matrix4, m8)));
                glEnableVertexAttribArray(SHADER_VERT_INSTANCEMATRIX_ROW3);
                glVertexAttribPointer(SHADER_VERT_INSTANCEMATRIX_ROW3, 4, GL_FLOAT, GL_FALSE,
                        16*sizeof(float), (void*) (OFFSET_OF(struct matrix4, m12)));

                glVertexAttribDivisor(SHADER_VERT_INSTANCEMATRIX_ROW0, 1);
                glVertexAttribDivisor(SHADER_VERT_INSTANCEMATRIX_ROW1, 1);
                glVertexAttribDivisor(SHADER_VERT_INSTANCEMATRIX_ROW2, 1);
                glVertexAttribDivisor(SHADER_VERT_INSTANCEMATRIX_ROW3, 1);
            }
            glBindVertexArray(0);
        }
        glUseProgram(0);

        model->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief Unloads the model from GPU memory.
 *
 * @param[inout] model
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
 * @brief Renders a model to the current OpenGL context. The model should have been loaded.
 *
 * @param[in] model
 */
void model_draw(struct model *model)
{
    if (model->material) {
        material_bind_uniform_blocks(model->material, model->shader);
        material_bind_textures(model->material, model->shader);
    }

    glUseProgram(model->shader->program);
    {
        glBindVertexArray(model->gpu_side.vao);
        {
            if (model->geometry) {
                glDrawElementsInstanced(GL_TRIANGLES, array_length(model->geometry->faces_array) * 3,
                        GL_UNSIGNED_INT, 0, array_length(model->instances_array));
            }
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
