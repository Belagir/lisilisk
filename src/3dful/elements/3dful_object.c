
#include "3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void object_send_space_uniforms(struct object object);
static void object_send_material_uniforms(struct object object);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param object
 * @param transform
 */
void object_transform(struct object *object, struct matrix4 transform)
{
    object->transform = transform;
}
/**
 * @brief
 *
 * @param object
 * @param geometry
 */
void object_geometry(struct object *object, struct geometry *geometry)
{
    object->geometry = geometry;
}

/**
 * @brief
 *
 * @param object
 * @param shader
 */
void object_shader(struct object *object, struct shader *shader)
{
    object->shader = shader;
}

/**
 * @brief
 *
 * @param object
 * @param color
 */
void object_color(struct object *object, f32 color[3])
{
    object->color[0] = color[0];
    object->color[1] = color[1];
    object->color[2] = color[2];
}

/**
 * @brief
 *
 * @param object
 */
void object_load(struct object *object)
{
    glGenVertexArrays(1, &object->gpu_side.vao);

    glBindVertexArray(object->gpu_side.vao);
    {
        glGenBuffers(1, &object->gpu_side.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, object->gpu_side.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                object->geometry->vertices->length * sizeof(*object->geometry->vertices->data),
                object->geometry->vertices->data, GL_STATIC_DRAW);

        glGenBuffers(1, &object->gpu_side.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gpu_side.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                object->geometry->faces->length * sizeof(*object->geometry->faces->data),
                object->geometry->faces->data, GL_STATIC_DRAW);

        glUseProgram(object->shader->program);
        {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*) 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*) (3 * sizeof(f32)));
            glEnableVertexAttribArray(1);
        }
        glUseProgram(0);
    }
    glBindVertexArray(0);
}

/**
 * @brief
 *
 * @param object
 */
void object_unload(struct object *object)
{
    glDeleteBuffers(1, &object->gpu_side.ebo);
    glDeleteBuffers(1, &object->gpu_side.vbo);
    glDeleteVertexArrays(1, &object->gpu_side.vao);

    object->gpu_side.ebo = 0;
    object->gpu_side.vbo = 0;
    object->gpu_side.vao = 0;
}

/**
 * @brief
 *
 * @param object
 */
void object_draw(struct object object)
{
    object_send_space_uniforms(object);
    object_send_material_uniforms(object);

    glUseProgram(object.shader->program);
    glBindVertexArray(object.gpu_side.vao);

    glDrawElements(GL_TRIANGLES, object.geometry->faces->length*3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param object
 */
static void object_send_space_uniforms(struct object object)
{
    GLint unif_name = -1;
    f32 tmp[16] = { };

    glUseProgram(object.shader->program);

    unif_name = glGetUniformLocation(object.shader->program, "MODEL_MATRIX");
    matrix4_to_array(object.transform, &tmp);
    glUniformMatrix4fv(unif_name, 1, GL_FALSE, (const GLfloat *) tmp);

    glUseProgram(0);
}

/**
 * @brief
 *
 * @param object
 */
static void object_send_material_uniforms(struct object object)
{
    GLint unif_name = -1;

    glUseProgram(object.shader->program);

    unif_name = glGetUniformLocation(object.shader->program, "COLOR");
    glUniform3fv(unif_name, 1, object.color);

    glUseProgram(0);
}
