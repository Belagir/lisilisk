
#include "3dful_core.h"

/**
 * @brief Sets how a material reflects ambient, global light.
 *
 * @param[inout] material
 * @param[in] ambient
 */
void material_ambient(struct material *material, f32 ambient[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->ambient[i] = ambient[i];
    }
}

/**
 * @brief Sets how a material diffuses light.
 *
 * @param[inout] material
 * @param[in] diffuse
 */
void material_diffuse(struct material *material, f32 diffuse[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->diffuse[i] = diffuse[i];
    }
}

/**
 * @brief Sets how a material reflects light.
 *
 * @param[inout] material
 * @param[in] specular
 */
void material_specular(struct material *material, f32 specular[4])
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->specular[i] = specular[i];
    }
}

/**
 * @brief Sets how clearly a material reflects specular lights.
 *
 * @param[inout] material
 * @param[in] shininess
 */
void material_shininess(struct material *material, float shininess)
{
    material->shininess = shininess;
}
