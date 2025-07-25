/**
 * @file 3dful_camera.c
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

/**
 * @brief Sets the position of a camera, and update the view matrix.
 * The camera will still be looking to its target position.
 *
 * @param[inout] camera Moved camera.
 * @param[in] pos new position.
 */
void camera_position(struct camera *camera, struct vector3 pos)
{
    camera->pos = pos;
    camera->view = matrix4_get_view_matrix(camera->pos,
            camera->target, VECTOR3_Y_POSITIVE);
}

/**
 * @brief Sets the field of view of the camera, and update the projection
 * matrix.
 *
 * @param[inout] camera Updated camera.
 * @param[in] fov New field of view, in degrees.
 */
void camera_fov(struct camera *camera, f32 fov)
{
    camera->fov = fov;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief Sets the target point of a camera, and update the view matrix.
 *
 * @param[inout] camera Updated camera.
 * @param[in] target New looking target.
 */
void camera_target(struct camera *camera, struct vector3 target)
{
    camera->target = target;
    camera->view = matrix4_get_view_matrix(camera->pos,
            camera->target, VECTOR3_Y_POSITIVE);
}

/**
 * @brief Sets the limits of the furstrum projected by a camera, and
 * updates the projection matrix.
 *
 * @param[inout] camera Updated camera.
 * @param[in] near Distance from the camera to the near plane.
 * @param[in] far Distance from the camera to the far plane.
 */
void camera_limits(struct camera *camera, f32 near, f32 far)
{
    camera->near = near;
    camera->far = far;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief Sets the aspect ration of the camera, and updates the projection
 * matrix.
 *
 * @param[inout] camera Updated camera.
 * @param[in] aspect New aspect ratio.
 */
void camera_aspect(struct camera *camera, f32 aspect)
{
    camera->aspect = aspect;
    camera->projection = matrix4_get_projection_matrix(camera->near,
            camera->far, camera->fov, camera->aspect);
}

/**
 * @brief Sends a camera's matrices to a shader.
 * The expected uniforms are :
 * - mat4 VIEW_MATRIX ;
 * - mat4 PROJECTION_MATRIX ;
 * - vec3 CAMERA_POS.
 *
 * @param[in] camera Source camera.
 * @param[in] shader Shader supporting the uniforms.
 */
void camera_send_uniforms(struct camera *camera, struct shader *shader)
{
    GLint uniform_name = -1;

    glUseProgram(shader->program);
    {
        uniform_name = glGetUniformLocation(shader->program,
                "VIEW_MATRIX");
        glUniformMatrix4fv(uniform_name, 1, GL_FALSE,
                (const GLfloat *) &camera->view);

        uniform_name = glGetUniformLocation(shader->program,
                "PROJECTION_MATRIX");
        glUniformMatrix4fv(uniform_name, 1, GL_FALSE,
                (const GLfloat *) &camera->projection);

        uniform_name = glGetUniformLocation(shader->program,
                "CAMERA_POS");
        glUniform3f(uniform_name, camera->pos.x, camera->pos.y, camera->pos.z);
    }
    glUseProgram(0);
}
