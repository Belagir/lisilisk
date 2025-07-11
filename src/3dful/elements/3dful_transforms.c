
#include "3dful_core.h"

/**
 * @brief Translates a transform matrix by some amount.
 *
 * @param[inout] matrix
 * @param[in] offset
 */
void transform_translate(struct matrix4 *matrix, vector3 offset)
{
    *matrix = matrix4_translate(*matrix, offset);
}

/**
 * @brief Translates a position vector by some amount.
 *
 * @param[inout] pos
 * @param[in] offset
 */
void position_translate(struct vector3 *pos, vector3 offset)
{
    *pos = vector3_add(*pos,  offset);
}
