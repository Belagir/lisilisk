
#include "3dful_core.h"

/**
 * @brief Sets how a material reflects ambient, global light.
 *
 * @param[inout] material
 * @param[in] ambient
 */
void material_ambient(struct material *material, vector3 ambient)
{
    material->ambient = ambient;
}

/**
 * @brief Sets how a material diffuses light.
 *
 * @param[inout] material
 * @param[in] diffuse
 */
void material_diffuse(struct material *material, vector3 diffuse)
{
    material->diffuse = diffuse;
}

/**
 * @brief Sets how a material reflects light.
 *
 * @param[inout] material
 * @param[in] specular
 */
void material_specular(struct material *material, vector3 specular)
{
    material->specular = specular;
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
