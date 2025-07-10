
#include "3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Sets the transform matrix of an object to an arbitrary one.
 *
 * @see matrix4_get_model_matrix()
 *
 * @param[inout] object
 * @param[in] transform
 */
void object_transform(struct object *object, struct matrix4 transform)
{
    object->transform = transform;
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
 * @brief Loads an object to the GPU with OpenGL.
 * Most of the trafic will be to tell which data is linked and sent to the shader.
 *
 * @param[inout] object
 */
void object_load(struct object *object)
{
    glGenVertexArrays(1, &object->gpu_side.vao);

    glBindVertexArray(object->gpu_side.vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, object->geometry->gpu_side.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->geometry->gpu_side.ebo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, pos));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) OFFSET_OF(struct vertex, normal));
        glEnableVertexAttribArray(1);

        material_send_uniforms(object->material, object->shader);
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
}

/**
 * @brief Renders an object to the current OpenGL context. The object should have been loaded.
 *
 * @param[in] object
 */
void object_draw(struct object object)
{
    object_send_uniforms(&object, object.shader);

    glUseProgram(object.shader->program);
    {
        glBindVertexArray(object.gpu_side.vao);
        {
            glDrawElements(GL_TRIANGLES, object.geometry->faces->length*3, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

/**
 * @brief
 *
 * @param object
 */
void object_send_uniforms(struct object *object, struct shader *shader)
{
    GLint unif_name = -1;

    glUseProgram(shader->program);
    {
        unif_name = glGetUniformLocation(shader->program, "MODEL_MATRIX");
        glUniformMatrix4fv(unif_name, 1, GL_FALSE, (const GLfloat *) &object->transform);
    }
    glUseProgram(0);
}
