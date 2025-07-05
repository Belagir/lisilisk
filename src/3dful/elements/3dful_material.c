
#include "3dful_core.h"

/**
 * @brief
 *
 * @param material
 * @param ambient
 */
void material_ambient(struct material *material, vector3 ambient)
{
    material->ambient = ambient;
}

/**
 * @brief
 *
 * @param material
 * @param diffuse
 */
void material_diffuse(struct material *material, vector3 diffuse)
{
    material->diffuse = diffuse;
}

/**
 * @brief
 *
 * @param material
 * @param specular
 */
void material_specular(struct material *material, vector3 specular)
{
    material->specular = specular;
}

/**
 * @brief
 *
 * @param material
 * @param shininess
 */
void material_shininess(struct material *material, float shininess)
{
    material->shininess = shininess;
}


