
#include "3dful_core.h"

/**
 * @brief
 *
 * @param camera
 * @param projection
 */
void camera_projection(struct camera *camera, struct matrix4 projection)
{
    camera->projection = projection;
}

/**
 * @brief
 *
 * @param camera
 * @param view
 */
void camera_view(struct camera *camera, struct matrix4 view)
{
    camera->view = view;
}
