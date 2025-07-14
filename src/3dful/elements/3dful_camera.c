
#include "3dful_core.h"

/**
 * @brief
 *
 * @param camera
 * @param pos
 */
void camera_position(struct camera *camera, struct vector3 pos)
{
    camera->pos = pos;
    camera->view = matrix4_get_view_matrix(camera->pos,
            camera->target, VECTOR3_Y_POSITIVE);
}

/**
 * @brief
 *
 * @param camera
 * @param fov
 */
void camera_fov(struct camera *camera, f32 fov)
{
    camera->fov = fov;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief
 *
 * @param camera
 * @param target
 */
void camera_target(struct camera *camera, struct vector3 target)
{
    camera->target = target;
    camera->view = matrix4_get_view_matrix(camera->pos,
            camera->target, VECTOR3_Y_POSITIVE);
}

/**
 * @brief
 *
 * @param camera
 * @param near
 * @param far
 */
void camera_limits(struct camera *camera, f32 near, f32 far)
{
    camera->near = near;
    camera->far = far;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief
 *
 * @param camera
 * @param aspect
 */
void camera_aspect(struct camera *camera, f32 aspect)
{
    camera->aspect = aspect;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief
 *
 * @param camera
 * @param shader
 */
void camera_send_uniforms(struct camera *camera, struct model *model)
{
    GLint uniform_name = -1;

    glUseProgram(model->shader->program);
    {
        uniform_name = glGetUniformLocation(model->shader->program, "VIEW_MATRIX");
        glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) &camera->view);

        uniform_name = glGetUniformLocation(model->shader->program, "PROJECTION_MATRIX");
        glUniformMatrix4fv(uniform_name, 1, GL_FALSE, (const GLfloat *) &camera->projection);

        uniform_name = glGetUniformLocation(model->shader->program, "CAMERA_POS");
        glUniform3f(uniform_name, camera->pos.x, camera->pos.y, camera->pos.z);
    }
    glUseProgram(0);

}
