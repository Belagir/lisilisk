
#include "3dful_core.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void material_set_sampler(struct material *material, u8 true_index, struct texture *texture);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const char * material_sampler_uniforms[MATERIAL_BASE_SAMPLERS_NUMBER] = {
        [MATERIAL_BASE_SAMPLER_AMBIENT_MASK]  = "ambient_mask",
        [MATERIAL_BASE_SAMPLER_SPECULAR_MASK] = "specular_mask",
        [MATERIAL_BASE_SAMPLER_DIFFUSE_MASK]  = "diffuse_mask",
        [MATERIAL_BASE_SAMPLER_EMISSIVE_MASK] = "emissive_mask",
        [MATERIAL_BASE_SAMPLER_TEXTURE]       = "base_texture",
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param material
 * @param texture
 */
void material_texture(struct material *material, struct texture *texture)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_TEXTURE, texture);
}

/**
 * @brief Sets how a material reflects ambient, global light.
 *
 * @param[inout] material
 * @param[in] ambient
 */
void material_ambient(struct material *material, f32 ambient[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.ambient[i] = ambient[i];
    }
    material->properties.ambient_strength = strength;
}

/**
 * @brief
 *
 * @param material
 * @param mask
 */
void material_ambient_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_AMBIENT_MASK, mask);
}

/**
 * @brief Sets how a material diffuses light.
 *
 * @param[inout] material
 * @param[in] diffuse
 */
void material_diffuse(struct material *material, f32 diffuse[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.diffuse[i] = diffuse[i];
    }
    material->properties.diffuse_strength = strength;
}

/**
 * @brief
 *
 * @param material
 * @param mask
 */
void material_diffuse_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_DIFFUSE_MASK, mask);
}

/**
 * @brief Sets how a material reflects light.
 *
 * @param[inout] material
 * @param[in] specular
 */
void material_specular(struct material *material, f32 specular[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.specular[i] = specular[i];
    }
    material->properties.specular_strength = strength;
}

/**
 * @brief
 *
 * @param material
 * @param mask
 */
void material_specular_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_SPECULAR_MASK, mask);
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
 * @param emission
 * @param strength
 */
void material_emissive(struct material *material, f32 emission[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.emissive[i] = emission[i];
    }
    material->properties.emissive_strength = strength;
}

/**
 * @brief
 *
 * @param material
 * @param mask
 */
void material_emissive_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_EMISSIVE_MASK, mask);
}

/**
 * @brief
 *
 * @param material
 * @param index
 * @param texture
 */
void material_custom_texture(struct material *material, u8 index, struct texture *texture)
{
    // there needs to be a name attached to the texture to set the uniform...
    (void) material;
    (void) index;
    (void) texture;

#if 0
    material_set_sampler(material, index + MATERIAL_BASE_SAMPLERS_NUMBER, texture);
#endif
}

/**
 * @brief
 *
 * @param material
 */
void material_load(struct material *material)
{
    loadable_add_user((struct loadable *) material);

    if (!loadable_needs_loading((struct loadable *) material)) {
        return;
    }

    glGenBuffers(1, &material->gpu_side.ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, material->gpu_side.ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(material->properties),
            &(material->properties), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    material->load_state.flags |= LOADABLE_FLAG_LOADED;

    for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
        if (material->samplers[i]) {
            texture_load(material->samplers[i]);
        }
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

        for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
            if (material->samplers[i]) {
                texture_unload(material->samplers[i]);
            }
        }
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

/**
 * @brief
 *
 * @param material
 * @param shader
 */
void material_bind_textures(struct material *material, struct model *model)
{
    glUseProgram(model->shader->program);
    {
        for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            if (material->samplers[i]) {
                glBindTexture(GL_TEXTURE_2D, material->samplers[i]->gpu_side.name);

                if (i < MATERIAL_BASE_SAMPLERS_NUMBER) {
                    glUniform1i(glGetUniformLocation(model->shader->program,
                            material_sampler_uniforms[i]), i);
                } else {
                    // custom texture name here
                }
            }
        }
    }
    glUseProgram(0);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void material_set_sampler(struct material *material, u8 true_index, struct texture *texture)
{
    if (material->samplers[true_index]) {
        texture_unload(material->samplers[true_index]);
    }

    if ((material->load_state.flags & LOADABLE_FLAG_LOADED) && texture) {
        texture_load(texture);
    }

    material->samplers[true_index] = texture;
}
