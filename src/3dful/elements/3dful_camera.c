
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
