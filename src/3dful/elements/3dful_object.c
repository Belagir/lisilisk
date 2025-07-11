
#include "3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param object
 */
void object_create(struct object *object)
{
    object->tr_instances = range_create_dynamic(make_system_allocator(),
            sizeof(*object->tr_instances->data), 64);
}

/**
 * @brief
 *
 * @param object
 */
void object_delete(struct object *object)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(object->tr_instances));
}

/**
 * @brief Links an object to a geometry. The object will be rendered with the geometry's mesh.
 *
 * @param[inout] object
 * @param[in] geometry
 */
void object_geometry(struct object *object, struct geometry *geometry)
{
    object->geometry = geometry;
}

/**
 * @brief Links an object to a shader. The object's geometry will be rendered with this shader.
 *
 * @param[inout] object
 * @param[in] shader
 */
void object_shader(struct object *object, struct shader *shader)
{
    object->shader = shader;
}

/**
 * @brief Links an object to a material. The object's shader will receive the material's data.
 *
 * @param[inout] object
 * @param[in] material
 */
void object_material(struct object *object, struct material *material)
{
    object->material = material;
}

/**
 * @brief
 *
 * @param object
 * @param tr
 */
void object_instantiate(struct object *object, struct matrix4 tr)
{
    object->tr_instances = range_ensure_capacity(make_system_allocator(),
            RANGE_TO_ANY(object->tr_instances), 1);
    range_push(RANGE_TO_ANY(object->tr_instances), &tr);
}

/**
 * @brief Loads an object to the GPU with OpenGL.
 * Most of the trafic will be to tell which data is linked and sent to the shader.
 *
 * @param[inout] object
 */
void object_load(struct object *object)
{
    // object's vao
    glGenVertexArrays(1, &object->gpu_side.vao);

    // instances data
    glGenBuffers(1, &object->gpu_side.vbo_instances);
    glBindBuffer(GL_ARRAY_BUFFER, object->gpu_side.vbo_instances);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(*object->tr_instances->data) * object->tr_instances->length,
            object->tr_instances->data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // binding scenario for this VAO
    glBindVertexArray(object->gpu_side.vao);
    {
        // vertex data from geometry
        glBindBuffer(GL_ARRAY_BUFFER, object->geometry->gpu_side.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->geometry->gpu_side.ebo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, normal));
        glEnableVertexAttribArray(1);

        // instances data
        glBindBuffer(GL_ARRAY_BUFFER, object->gpu_side.vbo_instances);
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

/**
 * @brief Unloads the object from GPU memory.
 *
 * @param[inout] object
 */
void object_unload(struct object *object)
{
    glDeleteVertexArrays(1, &object->gpu_side.vao);
    object->gpu_side.vao = 0;

    glDeleteBuffers(1, &object->gpu_side.vbo_instances);
    object->gpu_side.vbo_instances = 0;
}

/**
 * @brief Renders an object to the current OpenGL context. The object should have been loaded.
 *
 * @param[in] object
 */
void object_draw(struct object object)
{
    material_send_uniforms(object.material, &object);

    glUseProgram(object.shader->program);
    {
        glBindVertexArray(object.gpu_side.vao);
        {

            glDrawElementsInstanced(GL_TRIANGLES, object.geometry->faces->length*3,
                GL_UNSIGNED_INT, 0, object.tr_instances->length);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
