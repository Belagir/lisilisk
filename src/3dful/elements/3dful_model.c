
#include "3dful_core.h"

#include <ustd/array.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param model
 */
void model_create(struct model *model)
{
    model->tr_instances_array = array_create(make_system_allocator(), sizeof(*model->tr_instances_array), 64);
}

/**
 * @brief
 *
 * @param model
 */
void model_delete(struct model *model)
{
    array_destroy(make_system_allocator(), (void **) &model->tr_instances_array);
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
        geometry_load(geometry);
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
        material_load(material);
    }

    model->material = material;
}

/**
 * @brief
 *
 * @param model
 * @param tr
 */
void model_instantiate(struct model *model, struct matrix4 tr)
{
    array_ensure_capacity(make_system_allocator(), (void **) &model->tr_instances_array, 1);
    array_push(model->tr_instances_array, &tr);
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

        geometry_load(model->geometry);
        material_load(model->material);

        // model's vao
        glGenVertexArrays(1, &model->gpu_side.vao);

        // instances data
        glGenBuffers(1, &model->gpu_side.vbo_instances);
        glBindBuffer(GL_ARRAY_BUFFER, model->gpu_side.vbo_instances);
        glBufferData(GL_ARRAY_BUFFER,
                array_length(model->tr_instances_array) * sizeof(*model->tr_instances_array),
                model->tr_instances_array, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // binding scenario for this VAO
        glBindVertexArray(model->gpu_side.vao);
        {
            glUseProgram(model->shader->program);

            // vertex data from geometry
            glBindBuffer(GL_ARRAY_BUFFER, model->geometry->gpu_side.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->geometry->gpu_side.ebo);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, normal));
            glEnableVertexAttribArray(1);

            // instances data
            glBindBuffer(GL_ARRAY_BUFFER, model->gpu_side.vbo_instances);
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

        glDeleteBuffers(1, &model->gpu_side.vbo_instances);
        model->gpu_side.vbo_instances = 0;

        model->load_state.flags &= ~LOADABLE_FLAG_LOADED;

        geometry_unload(model->geometry);
        material_unload(model->material);
    }
}

/**
 * @brief Renders a model to the current OpenGL context. The model should have been loaded.
 *
 * @param[in] model
 */
void model_draw(struct model model)
{
    material_bind_uniform_blocks(model.material, &model);
    material_bind_textures(model.material, &model);

    glUseProgram(model.shader->program);
    {
        glBindVertexArray(model.gpu_side.vao);
        {
            glDrawElementsInstanced(GL_TRIANGLES, array_length(model.geometry->faces_array) * 3,
                GL_UNSIGNED_INT, 0, array_length(model.tr_instances_array));
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
