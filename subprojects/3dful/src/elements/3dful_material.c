
#include "3dful_core.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void material_set_sampler(struct material *material, u8 true_index,
        struct texture *texture);
static void material_update_ubo(struct material *material, size_t offset,
        size_t size);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Map of sampler enumeration values to their expected name in a shade
 * program.
 */
static const char * material_sampler_uniforms[MATERIAL_BASE_SAMPLERS_NUMBER] = {
        [MATERIAL_BASE_SAMPLER_AMBIENT_MASK]  = "ambient_mask",
        [MATERIAL_BASE_SAMPLER_SPECULAR_MASK] = "specular_mask",
        [MATERIAL_BASE_SAMPLER_DIFFUSE_MASK]  = "diffuse_mask",
        [MATERIAL_BASE_SAMPLER_EMISSIVE_MASK] = "emissive_mask",
        [MATERIAL_BASE_SAMPLER_TEXTURE]       = "base_texture",
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param material
 */
void material_create(struct material *material, const struct material *source)
{
    *material = (struct material) { 0 };
    material->added_uniforms = hashmap_create(make_system_allocator(),
                    sizeof(*material->added_uniforms), 32);

    if (source) {
        for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
            material->samplers[i] = source->samplers[i];
        }
        material->properties = source->properties;
    }
}

/**
 * @brief
 *
 * @param material
 */
void material_delete(struct material *material)
{
    hashmap_destroy(make_system_allocator(),
            (HASHMAP_ANY *) &material->added_uniforms);
    *material = (struct material) { 0 };
}

/**
 * @brief Sets the base texture image of a material.
 * Usually the background skin of a model.
 *
 * @param[inout] material Modified material.
 * @param[in] texture 2D texture.
 */
void material_texture(struct material *material, struct texture *texture)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_TEXTURE, texture);
}

/**
 * @brief Sets how a material reflects ambient, global light.
 *
 * @param[inout] material Modified material.
 * @param[in] ambient Color albedo for ambient light.
 */
void material_ambient(struct material *material, f32 ambient[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.ambient[i] = ambient[i];
    }
    material->properties.ambient_strength = strength;

    material_update_ubo(material,
            OFFSET_OF(struct material_properties, ambient),
            sizeof(material->properties.ambient));
    material_update_ubo(material,
            OFFSET_OF(struct material_properties, ambient_strength),
            sizeof(material->properties.ambient_strength));
}

/**
 * @brief Sets the ambient light texture 2D mask.
 *
 * @param[inout] material Modified material.
 * @param[in] texture 2D lightmap.
 */
void material_ambient_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_AMBIENT_MASK, mask);
}

/**
 * @brief Sets how a material diffuses light.
 *
 * @param[inout] material Modified material.
 * @param[in] diffuse Color albedo for diffuse highlights.
 * @param[in] strength Strength of the diffuse highlights.
 */
void material_diffuse(struct material *material, f32 diffuse[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.diffuse[i] = diffuse[i];
    }
    material->properties.diffuse_strength = strength;

    material_update_ubo(material,
            OFFSET_OF(struct material_properties, diffuse),
            sizeof(material->properties.diffuse));
    material_update_ubo(material,
            OFFSET_OF(struct material_properties, diffuse_strength),
            sizeof(material->properties.diffuse_strength));
}

/**
 * @brief Sets the diffuse light texture 2D mask.
 *
 * @param[inout] material Modified material.
 * @param[in] texture 2D lightmap.
 */
void material_diffuse_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_DIFFUSE_MASK, mask);
}

/**
 * @brief Sets how a material reflects light.
 *
 * @param[inout] material Modified material.
 * @param[in] diffuse Color albedo for specular highlights.
 * @param[in] strength Strength of the specular highlights.
 */
void material_specular(struct material *material, f32 specular[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.specular[i] = specular[i];
    }
    material->properties.specular_strength = strength;

    material_update_ubo(material,
            OFFSET_OF(struct material_properties, specular),
            sizeof(material->properties.specular));
    material_update_ubo(material,
            OFFSET_OF(struct material_properties, specular_strength),
            sizeof(material->properties.specular_strength));
}

/**
 * @brief Sets the specular light texture 2D mask.
 *
 * @param[inout] material Modified material.
 * @param[in] texture 2D lightmap.
 */
void material_specular_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_SPECULAR_MASK, mask);
}

/**
 * @brief Sets how strongly a material reflects specular lights.
 *
 * @param[inout] material Modified material.
 * @param[in] shininess Shininess value.
 */
void material_shininess(struct material *material, float shininess)
{
    material->properties.shininess = shininess;

    material_update_ubo(material,
            OFFSET_OF(struct material_properties, shininess),
            sizeof(material->properties.shininess));
}

/**
 * @brief Sets how a material emits light.
 *
 * @param[inout] material Modified material.
 * @param[in] diffuse Color of the emission.
 * @param[in] strength Strength of the emission.
 */
void material_emissive(struct material *material, f32 emission[3], f32 strength)
{
    for (size_t i = 0 ; i < 4 ; i++) {
        material->properties.emissive[i] = emission[i];
    }
    material->properties.emissive_strength = strength;

    material_update_ubo(material,
            OFFSET_OF(struct material_properties, emissive),
            sizeof(material->properties.emissive));
    material_update_ubo(material,
            OFFSET_OF(struct material_properties, emissive_strength),
            sizeof(material->properties.emissive_strength));
}

/**
 * @brief Sets the emission 2D texture.
 *
 * @param[inout] material Modified material.
 * @param[in] texture 2D lightmap.
 */
void material_emissive_mask(struct material *material, struct texture *mask)
{
    material_set_sampler(material, MATERIAL_BASE_SAMPLER_EMISSIVE_MASK, mask);
}

/**
 * @brief
 *
 * @param material
 * @param name
 */
bool material_has_uniform(struct material *material, const char *name)
{
    return (hashmap_index_of(material->added_uniforms, name)
                < array_length(material->added_uniforms));
}

/**
 * @brief
 *
 * @param material
 * @param name
 * @param nb
 */
void material_add_uniform_float(struct material *material, const char *name,
        size_t nb)
{
    if (!name || !nb) {
        return;
    }

    hashmap_ensure_capacity(make_system_allocator(),
            (HASHMAP_ANY *) &material->added_uniforms, 1);
    hashmap_set(material->added_uniforms, name, &(struct material_user_uniform) {
            .name = name,
            .nb = nb,
            .base_type = MATERIAL_UNIFORM_FLOAT1 + (nb - 1),
            .value = { },
    });
}

/**
 * @brief
 *
 * @param material
 * @param name
 */
void material_add_uniform_texture(struct material *material, const char *name)
{
    if (!name) {
        return;
    }

    hashmap_ensure_capacity(make_system_allocator(),
            (HASHMAP_ANY *) &material->added_uniforms, 1);
    hashmap_set(material->added_uniforms, name, &(struct material_user_uniform) {
            .name = name,
            .nb = material->added_textures_number,
            .base_type = MATERIAL_UNIFORM_TEXTURE2D,
            .value = { },
    });

    material->added_textures_number += 1;
}

/**
 * @brief
 *
 * @param material
 * @param name
 * @param data
 */
void material_set_uniform(struct material *material, const char *name,
        void *data)
{
    size_t pos = hashmap_index_of(material->added_uniforms, name);
    struct material_user_uniform *uniform = { };

    if (pos >= array_length(material->added_uniforms)) {
        return;
    }

    uniform = material->added_uniforms + pos;
    switch (uniform->base_type) {
        case MATERIAL_UNIFORM_FLOAT1:
            bytewise_copy(&uniform->value, data,
                    sizeof(uniform->value.as_float1));
            break;
        case MATERIAL_UNIFORM_FLOAT2:
            bytewise_copy(&uniform->value, data,
                    sizeof(uniform->value.as_float2));
            break;
        case MATERIAL_UNIFORM_FLOAT3:
            bytewise_copy(&uniform->value, data,
                    sizeof(uniform->value.as_float3));
            break;
        case MATERIAL_UNIFORM_FLOAT4:
            bytewise_copy(&uniform->value, data,
                    sizeof(uniform->value.as_float4));
            break;
        case MATERIAL_UNIFORM_TEXTURE2D:
            bytewise_copy(&uniform->value, data,
                    sizeof(uniform->value.as_texture));
            if ((material->load_state.flags & LOADABLE_FLAG_LOADED)
                        && (uniform->value.as_texture)) {
                texture_load(uniform->value.as_texture);
            }
            break;
    }

}

/**
 * @brief Mark the material as being needed to be loaded to the GPU so it can
 * be usable.
 *
 * @param[inout] material Loaded material.
 */
void material_load(struct material *material)
{
    struct material_user_uniform uniform = { };

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

    for (size_t i = 0 ; i < array_length(material->added_uniforms) ; i++) {
        uniform = material->added_uniforms[i];
        switch (uniform.base_type) {
            case MATERIAL_UNIFORM_FLOAT1:
                break;
            case MATERIAL_UNIFORM_FLOAT2:
                break;
            case MATERIAL_UNIFORM_FLOAT3:
                break;
            case MATERIAL_UNIFORM_FLOAT4:
                break;
            case MATERIAL_UNIFORM_TEXTURE2D:
                if (uniform.value.as_texture) {
                    texture_load(uniform.value.as_texture);
                }
                break;
        }
    }
}

/**
 * @brief Marks the material as no longer being needed on the GPU.
 *
 * @param[inout] material Unloaded material.
 */
void material_unload(struct material *material)
{
    struct material_user_uniform uniform = { };

    loadable_remove_user((struct loadable *) material);

    if (!loadable_needs_unloading((struct loadable *) material)) {
        return;
    }

    glDeleteBuffers(1, &material->gpu_side.ubo);
    material->load_state.flags &= ~LOADABLE_FLAG_LOADED;

    for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
        if (material->samplers[i]) {
            texture_unload(material->samplers[i]);
        }
    }

    for (size_t i = 0 ; i < array_length(material->added_uniforms) ; i++) {
        uniform = material->added_uniforms[i];
        switch (uniform.base_type) {
            case MATERIAL_UNIFORM_FLOAT1:
                break;
            case MATERIAL_UNIFORM_FLOAT2:
                break;
            case MATERIAL_UNIFORM_FLOAT3:
                break;
            case MATERIAL_UNIFORM_FLOAT4:
                break;
            case MATERIAL_UNIFORM_TEXTURE2D:
                if (uniform.value.as_texture) {
                    texture_unload(uniform.value.as_texture);
                }
                break;
        }
    }

}

/**
 * @brief
 *
 * @param material
 * @param shader
 */
void material_send_uniforms(struct material *material, struct shader *shader)
{
    GLint location = 0;
    struct material_user_uniform uniform = { };

    glUseProgram(shader->program);

    for (size_t i = 0 ; i < array_length(material->added_uniforms) ; i++) {
        uniform = material->added_uniforms[i];

        location = glGetUniformLocation(shader->program, uniform.name);
        if (location == -1) {
            continue;
        }

        switch (material->added_uniforms[i].base_type) {
            case MATERIAL_UNIFORM_FLOAT1:
                glUniform1fv(location, uniform.nb, (const GLfloat *) &uniform.value.as_float1);
                break;
            case MATERIAL_UNIFORM_FLOAT2:
                glUniform2fv(location, uniform.nb, (const GLfloat *) &uniform.value.as_float2);
                break;
            case MATERIAL_UNIFORM_FLOAT3:
                glUniform3fv(location, uniform.nb, (const GLfloat *) &uniform.value.as_float3);
                break;
            case MATERIAL_UNIFORM_FLOAT4:
                glUniform4fv(location, uniform.nb, (const GLfloat *) &uniform.value.as_float4);
                break;

            case MATERIAL_UNIFORM_TEXTURE2D:
                break;
        }
    }

    glUseProgram(0);
}

/**
 * @brief Bind the buffers loaded by the material to the opengl context so a
 * shader can take them as inputs.
 *
 * @param[in] material Target loaded material.
 * @param[in] shader Shader taking the inputs.
 */
void material_bind_uniform_blocks(struct material *material,
        struct shader *shader)
{
    glUseProgram(shader->program);

    GLint block_name = -1;

    block_name = glGetUniformBlockIndex(shader->program, "BLOCK_MATERIAL");
    glUniformBlockBinding(shader->program, block_name, SHADER_UBO_MATERIAL);

    glBindBuffer(GL_UNIFORM_BUFFER, material->gpu_side.ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, block_name, material->gpu_side.ubo);

    glUseProgram(0);
}

/**
 * @brief Bind the textures loaded by the material to the opengl context so a
 * shader can take them as inputs
 *
 * @param[in] material Target loaded material.
 * @param[in] shader Shader taking the textures.
 */
void material_bind_textures(struct material *material, struct shader *shader)
{
    GLint location = 0;
    struct material_user_uniform uniform = { };

    glUseProgram(shader->program);
    for (size_t i = 0 ; i < COUNT_OF(material->samplers) ; i++) {
        location = glGetUniformLocation(shader->program,
                material_sampler_uniforms[i]);

        glActiveTexture(GL_TEXTURE0 + i);
        if (material->samplers[i]) {
            glBindTexture(GL_TEXTURE_2D,
                    material->samplers[i]->gpu_side.name);
            glUniform1i(location, i);
        }
    }

    for (size_t i = 0 ; i < array_length(material->added_uniforms) ; i++) {
        uniform = material->added_uniforms[i];

        location = glGetUniformLocation(shader->program, uniform.name);
        if (location == -1) {
            continue;
        }

        switch (material->added_uniforms[i].base_type) {
            case MATERIAL_UNIFORM_FLOAT1:
                break;
            case MATERIAL_UNIFORM_FLOAT2:
                break;
            case MATERIAL_UNIFORM_FLOAT3:
                break;
            case MATERIAL_UNIFORM_FLOAT4:
                break;

            case MATERIAL_UNIFORM_TEXTURE2D:
                glActiveTexture(GL_TEXTURE0 + MATERIAL_BASE_SAMPLERS_NUMBER + uniform.nb);
                if (uniform.value.as_texture) {
                    glBindTexture(GL_TEXTURE_2D,uniform.value.as_texture->gpu_side.
                            name);
                    glUniform1i(location, MATERIAL_BASE_SAMPLERS_NUMBER + uniform.nb);
                }
                break;
        }
    }

    glUseProgram(0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Changes the texture bound to a sampler index, loading and unloading
 * textures as needed.
 *
 * @param[inout] material
 * @param[in] true_index
 * @param[inout] texture
 */
static void material_set_sampler(struct material *material, u8 true_index,
         struct texture *texture)
{
    if (material->load_state.flags & LOADABLE_FLAG_LOADED) {
        if (material->samplers[true_index]) {
            texture_unload(material->samplers[true_index]);
        }
        if (texture) {
            texture_load(texture);
        }
    }

    material->samplers[true_index] = texture;
}

/**
 * @brief Updates the Uniform Buffer Object of the material if needed.
 *
 * @param[inout] material
 * @param[in] offset
 * @param[in] size
 */
static void material_update_ubo(struct material *material, size_t offset,
         size_t size)
{
    if (!(material->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, material->gpu_side.ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size,
            (byte *) &(material->properties) + offset);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
