
#include "3dful_core.h"

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
    object->shading = shader;
}

/**
 * @brief
 *
 * @param object
 */
void object_load(struct object *object)
{
    glGenVertexArrays(1, &object->vao);
    glBindVertexArray(object->vao);

    glGenBuffers(1, &object->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
    glBufferData(GL_ARRAY_BUFFER,
            object->geometry->vertices->length * sizeof(*object->geometry->vertices->data),
            object->geometry->vertices->data, GL_STATIC_DRAW);

    glUseProgram(object->shading->program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(0);
    glBindVertexArray(0);
}

/**
 * @brief
 *
 * @param object
 */
void object_unload(struct object *object)
{
    glDeleteBuffers(1, &object->ebo);
    glDeleteBuffers(1, &object->vbo);
    glDeleteVertexArrays(1, &object->vao);

    object->ebo = 0;
    object->vbo = 0;
    object->vao = 0;
}

/**
 * @brief
 *
 * @param object
 */
void object_draw(struct object object)
{
    glUseProgram(object.shading->program);
    glBindVertexArray(object.vao);
    glDrawArrays(GL_TRIANGLES, 0, object.geometry->vertices->length);
}
