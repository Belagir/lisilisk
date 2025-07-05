
#include "3dful_core.h"

/**
 * @brief
 *
 * @param matrix
 * @param offset
 */
void transform_translate(struct matrix4 *matrix, vector3 offset)
{
    *matrix = matrix_translate(*matrix, offset);
}

/**
 * @brief
 *
 * @param pos
 * @param offset
 */
void position_translate(struct vector3 *pos, vector3 offset)
{
    *pos = vector3_add(*pos,  offset);
}
