
#include "3dful_core.h"

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static size_t model_instance_index_of(struct model *model, handle_t handle);
static i32 handle_compare(const void *lhs, const void *rhs);
static void model_vbo_instances_sync_capacity(struct model *model);
static void model_vbo_instances_load(struct model *model, size_t vbo_size);
static void model_vbo_instances_sync_element(struct model *model, size_t index);

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

            .tr_instances_array = array_create(make_system_allocator(), sizeof(*model->tr_instances_array), 32),
            .handles_array = array_create(make_system_allocator(), sizeof(*model->handles_array), 32),

            .gpu_side = { 0 },
    };
}

/**
 * @brief
 *
 * @param model
 */
void model_delete(struct model *model)
{
    array_destroy(make_system_allocator(), (void **) &model->tr_instances_array);
    array_destroy(make_system_allocator(), (void **) &model->handles_array);

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
    static u16 static_id_counter = 1;

    if (static_id_counter == UINT16_MAX) {
        *out_handle = 0;
        return;
    }

    static_id_counter += 1;
    *out_handle = static_id_counter;
    
    array_ensure_capacity(make_system_allocator(), (void **) &model->handles_array, 1);
    array_push(model->handles_array, out_handle);
    
    array_ensure_capacity(make_system_allocator(), (void **) &model->tr_instances_array, 1);
    array_push(model->tr_instances_array, &MATRIX4_IDENTITY);

    model_vbo_instances_sync_capacity(model);
    model_vbo_instances_sync_element(model, array_length(model->tr_instances_array)-1);
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
    size_t idx = model_instance_index_of(model, handle);

    if (idx == array_length(model->tr_instances_array)) {
        return;
    }

    model->tr_instances_array[idx] = tr;
    
    model_vbo_instances_sync_element(model, idx);
}

/**
 * @brief
 *
 * @param model
 * @param handle
 */
void model_instance_remove(struct model *model, handle_t handle)
{
    size_t idx = model_instance_index_of(model, handle);

    if (idx == array_length(model->handles_array)) {
        return;
    }

    array_remove_swapback(model->handles_array, idx);
    array_remove_swapback(model->tr_instances_array, idx);

    model_vbo_instances_sync_element(model, idx);
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

        // model's vao
        glGenVertexArrays(1, &model->gpu_side.vao);

        // instances data
        glGenBuffers(1, &model->gpu_side.vbo_instances);
        model_vbo_instances_load(model, array_capacity(model->tr_instances_array));

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

        glDeleteBuffers(1, &model->gpu_side.vbo_instances);
        model->gpu_side.vbo_instances = 0;

        model->load_state.flags &= ~LOADABLE_FLAG_LOADED;

        if (model->geometry) geometry_unload(model->geometry);
        if (model->material) material_unload(model->material);
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
                        GL_UNSIGNED_INT, 0, array_length(model->tr_instances_array));
            }
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param model
 * @param handle
 * @return size_t
 */
static size_t model_instance_index_of(struct model *model, handle_t handle)
{
    size_t pos = 0;

    if (array_find(model->handles_array, &handle_compare, &handle, &pos)) {
        return pos;
    }

    return array_length(model->handles_array);
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 handle_compare(const void *lhs, const void *rhs)
{
    handle_t handle_lhs = *(handle_t *) lhs;
    handle_t handle_rhs = *(handle_t *) rhs;

    return (handle_lhs > handle_rhs) - (handle_lhs < handle_rhs);
}

/**
 * @brief 
 * 
 * @param model 
 */
static void model_vbo_instances_sync_capacity(struct model *model)
{
    GLint buffer_size = 0;

    if (!(model->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, model->gpu_side.vbo_instances);
    {
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (buffer_size != (GLint) array_capacity(model->tr_instances_array)) {
        model_vbo_instances_load(model, array_capacity(model->tr_instances_array));
    }
}

/**
 * @brief 
 * 
 * @param model 
 */
static void model_vbo_instances_load(struct model *model, size_t vbo_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, model->gpu_side.vbo_instances);
    {
        glBufferData(GL_ARRAY_BUFFER,
                vbo_size * sizeof(*model->tr_instances_array),
                model->tr_instances_array, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief 
 * 
 * @param model 
 * @param tr 
 */
static void model_vbo_instances_sync_element(struct model *model, size_t index)
{
    if (!(model->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, model->gpu_side.vbo_instances);
    {
        glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(*model->tr_instances_array), 
                sizeof(*model->tr_instances_array), model->tr_instances_array + index);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
