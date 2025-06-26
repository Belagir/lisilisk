
#include "opengl_scenes.h"

/**
 * @brie
 *
 * @param alloc
 * @return struct object
 */
struct geometry create_geometry_empty(struct allocator alloc)
{
    struct geometry new_geometry = {
            .name = range_create_dynamic(alloc, sizeof(&new_geometry.name->data), 64),
            .vertices = range_create_dynamic(alloc, sizeof(&new_geometry.vertices->data), 128),
            .colors = range_create_dynamic(alloc, sizeof(&new_geometry.colors->data), 128),
    };

    return new_geometry;
}

/**
 * @brief
 *
 * @param alloc
 * @param geometry
 */
void destroy_geometry(struct allocator alloc, struct geometry *geometry)
{
    if (!geometry) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->colors));

    *geometry = (struct geometry) { 0 };
}

/**
 * @brief Create a object from geometry object
 *
 * @param geometry
 * @return struct object
 */
struct object create_object_from_geometry(struct geometry geometry)
{
    struct object new_object = { };

    (void) geometry;

    glGenVertexArrays(1, &new_object.vao);

    glBindVertexArray(new_object.vao);

    glGenBuffers(2, new_object.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, new_object.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * geometry.vertices->length, geometry.vertices->data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, new_object.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat) * geometry.colors->length, geometry.colors->data, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    new_object.vertices_nb = geometry.vertices->length;

    glBindVertexArray(0);

    return new_object;
}

/**
 * @brief
 *
 * @param object
 */
void destroy_object(struct object *object)
{
    if (!object) {
        return;
    }

    glDeleteBuffers(2, object->vbo);
    glDeleteVertexArrays(1, &object->vao);

    *object = (struct object) { 0 };
}

/**
 * @brief
 *
 * @param object
 */
void render_object(struct object object)
{
    glBindVertexArray(object.vao);
    glUseProgram(object.shader_program);

    glDrawArrays(GL_LINE_LOOP, 0, object.vertices_nb);

    glUseProgram(0);
    glBindVertexArray(0);
}
