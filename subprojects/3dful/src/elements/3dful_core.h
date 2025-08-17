/**
 * @file 3dful_core.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef CORE_3DFUL_H__
#define CORE_3DFUL_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/math2d.h>
#include <ustd/math3d.h>

#include <3dful.h>
#include "../inout/file_operations.h"
#include "../dynamic_data/3dful_dynamic_data.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Assigns integer values to semantic names for Uniform Buffer Objects
 * binding indices.
 * @warning ubo locations are messed up my other computer !!! what ?
 */
enum shader_ubo_binding {
    SHADER_UBO_MATERIAL,
    SHADER_UBO_LIGHT_DIREC,
    SHADER_UBO_LIGHT_POINT,
};

/**
 * @brief  Assigns integer values to semantic names for vertex pointers.
 *
 */
enum shader_vertex_binding {
    SHADER_VERT_POS,
    SHADER_VERT_NORMAL,
    SHADER_VERT_UV,
    SHADER_VERT_INSTANCEPOSITION,
    SHADER_VERT_INSTANCESCALE,
    SHADER_VERT_INSTANCEROTATION,
};

/**
 * @brief This enumeration is a direct correspondance with the multiple
 * uniform sampler2D layout indices found in the `frag_head.glsl`.
 */
enum material_base_sampler {
    MATERIAL_BASE_SAMPLER_AMBIENT_MASK,
    MATERIAL_BASE_SAMPLER_SPECULAR_MASK,
    MATERIAL_BASE_SAMPLER_DIFFUSE_MASK,
    MATERIAL_BASE_SAMPLER_EMISSIVE_MASK,
    MATERIAL_BASE_SAMPLER_TEXTURE,

    MATERIAL_BASE_SAMPLERS_NUMBER,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// GEOMETRY --------------------------------------------------------------------

void geometry_push_vertex(struct geometry *geometry, u32 *out_idx);
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos);
void geometry_vertex_normal(struct geometry *geometry, size_t idx,
        vector3 normal);
void geometry_vertex_uv(struct geometry *geometry, size_t idx, vector2 uv);

void geometry_push_face(struct geometry *geometry, u32 *out_idx);
void geometry_face_indices(struct geometry *geometry, size_t idx,
        u32 indices[3u]);

void geometry_load(struct geometry *geometry);
void geometry_unload(struct geometry *geometry);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// TEXTURE ---------------------------------------------------------------------

void texture_load(struct texture *texture);
void texture_unload(struct texture *texture);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// MATERIAL --------------------------------------------------------------------

void material_load(struct material *material);
void material_unload(struct material *material);
void material_send_uniforms(struct material *material, struct shader *shader);
void material_bind_uniform_blocks(struct material *material,
        struct shader *shader);
void material_bind_textures(struct material *material, struct shader *shader);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// MODEL -----------------------------------------------------------------------

void model_load(struct model *model);
void model_unload(struct model *model);
void model_draw(struct model *model);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CAMERA ----------------------------------------------------------------------

void camera_send_uniforms(struct camera *camera, struct shader *shader);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// ENVIRONMENT -----------------------------------------------------------------

void environment_draw(struct environment *env);
void environment_send_uniforms(struct environment *env, struct shader *shader);

void environment_load(struct environment *env);
void environment_unload(struct environment *env);

#endif
