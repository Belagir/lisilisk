
#include "3dful_core.h"

/**
 * @brief Sets a camera's projection to an arbitrary matrix.
 * This matrix governs how a camera distorts space it sees.
 *
 * @see matrix4_get_projection_matrix
 *
 * @param[inout] camera
 * @param[in] projection
 */
void camera_projection(struct camera *camera, struct matrix4 projection)
{
    camera->projection = projection;
}

/**
 * @brief Sets a camera's view to an arbitrary matrix.
 * This matrix governs where a camera sees from.
 *
 * @see matrix4_get_view_matrix
 *
 * @param[inout] camera
 * @param[in] view
 */
void camera_view(struct camera *camera, struct matrix4 view)
{
    camera->view = view;
}

/**
 * @brief
 *
 * @param camera
 * @param shader
 */
void camera_send_uniforms(struct camera *camera, struct object *object)
{
    vector3 cam_pos = { };
    GLint uniform_name = -1;

    glUseProgram(object->shader->program);
    {
        glBindVertexArray(object->gpu_side.vao);
        {
            uniform_name = glGetUniformLocation(object->shader->program, "VIEW_MATRIX");
            glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) &camera->view);

            uniform_name = glGetUniformLocation(object->shader->program, "PROJECTION_MATRIX");
            glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) &camera->projection);

            uniform_name = glGetUniformLocation(object->shader->program, "CAMERA_POS");
            cam_pos = matrix4_origin(camera->view);
            glUniform3f(uniform_name, cam_pos.x, cam_pos.y, cam_pos.z);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
