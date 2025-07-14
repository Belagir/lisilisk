
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
        material->properties.ambient[i] = ambient[i];
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
        material->properties.diffuse[i] = diffuse[i];
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
        material->properties.specular[i] = specular[i];
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
    material->properties.shininess = shininess;
}

/**
 * @brief
 *
 * @param material
 */
void material_load(struct material *material)
{
    loadable_add_user((struct loadable *) material);

    if (loadable_needs_loading((struct loadable *) material)) {

        glGenBuffers(1, &material->gpu_side.ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, material->gpu_side.ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(material->properties),
                &(material->properties), GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        material->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief
 *
 * @param material
 */
void material_unload(struct material *material)
{
    loadable_remove_user((struct loadable *) material);

    if (loadable_needs_unloading((struct loadable *) material)) {

        glDeleteBuffers(1, &material->gpu_side.ubo);
        material->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }

}

/**
 * @brief
 *
 * @param material
 * @param shader
 */
void material_bind_uniform_blocks(struct material *material, struct model *model)
{
    glUseProgram(model->shader->program);
    {
        GLint block_name = -1;

        block_name = glGetUniformBlockIndex(model->shader->program, "BLOCK_MATERIAL");
        glUniformBlockBinding(model->shader->program, block_name, SHADER_UBO_MATERIAL);

        glBindBuffer(GL_UNIFORM_BUFFER, material->gpu_side.ubo);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_name, material->gpu_side.ubo);
    }
    glUseProgram(0);
}
