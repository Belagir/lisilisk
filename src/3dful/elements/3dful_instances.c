
#include "3dful_core.h"

/**
 * @brief
 *
 * @param instances
 */
void instances_create(struct instances *instances)
{
    instances->transforms = range_create_dynamic(make_system_allocator(), sizeof(*instances->transforms->data), 64);
}

/**
 * @brief
 *
 * @param instances
 */
void instances_delete(struct instances *instances)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(instances->transforms));
}


/**
 * @brief
 *
 * @param instances
 * @param object
 */
void instances_of(struct instances *instances, struct object *object)
{
    instances->target = object;
}

/**
 * @brief
 *
 * @param instances
 */
void instances_load(struct instances *instances)
{
    glGenBuffers(1, &instances->gpu_side.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, instances->gpu_side.vbo);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(*instances->transforms->data) * instances->transforms->length,
            instances->transforms->data, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(instances->target->gpu_side.vao);

    glBindBuffer(GL_ARRAY_BUFFER, instances->gpu_side.vbo);
    glEnableVertexAttribArray(SHADER_VERT_INSTANCE);
    glVertexAttribPointer(SHADER_VERT_INSTANCE, 16, GL_FLOAT, GL_FALSE, sizeof(*instances->transforms->data), (void*)0);
    glVertexAttribDivisor(SHADER_VERT_INSTANCE, 1);

    glBindVertexArray(0);
}

/**
 * @brief
 *
 * @param instances
 */
void instances_unload(struct instances *instances)
{
    glDeleteBuffers(1, &instances->gpu_side.vbo);
    instances->gpu_side.vbo = 0;
}

/**
 * @brief
 *
 * @param instances
 */
void instances_draw(struct instances *instances)
{
    object_send_uniforms(instances->target, instances->target->shader);
    material_send_uniforms(instances->target->material, instances->target->shader);

    glUseProgram(instances->target->shader->program);
    {
        glBindVertexArray(instances->target->gpu_side.vao);
        {
            glDrawElementsInstanced(GL_TRIANGLES, instances->target->geometry->faces->length*3,
                GL_UNSIGNED_INT, 0, instances->transforms->length);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);

}

/**
 * @brief
 *
 * @param instances
 * @param transform
 */
void instances_push(struct instances *instances, struct matrix4 transform)
{
    instances->transforms = range_ensure_capacity(make_system_allocator(), RANGE_TO_ANY(instances->transforms), 1);
    range_push(RANGE_TO_ANY(instances->transforms), &transform);
}
