
#include "3dful_core.h"

/**
 * @brief 
 * 
 * @param geometry 
 * @param frag_shader 
 * @param vert_shader 
 * @return struct object 
 */
struct object object_create(struct geometry geometry, struct shader_program shaders)
{
    struct object object = { 0 };

    if (!geometry.colors || !geometry.vertices || !shaders.program) {
        return (struct object) { 0 };
    }

    glGenVertexArrays(1, &(object.vao));
    glBindVertexArray(object.vao);

    glGenBuffers(2, object.vbo);

    // positions
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, geometry.vertices->length * sizeof(*geometry.vertices->data), geometry.vertices->data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, sizeof(*geometry.vertices->data) / sizeof(*geometry.vertices->data->array), GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // colors
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, geometry.colors->length * sizeof(*geometry.colors->data), geometry.colors->data, GL_STATIC_DRAW);
    glVertexAttribPointer(1, sizeof(*geometry.colors->data) / sizeof(*geometry.colors->data->array), GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    object.shading = shaders;

    return object;
}

/**
 * @brief 
 * 
 * @param object 
 */
void object_destroy(struct object *object)
{
    if (!object) {
        return;
    }

    glDeleteBuffers(2, object->vbo);
    glDeleteVertexArrays(1, &object->vao);

    *object = (struct object) { 0 };
}
