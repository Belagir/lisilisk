/**
 * @file 3dful_texture.c
 * @author Gabriel Bédat
 * @brief Implementation of everything texture-related.
 * @version 0.1
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void texture_load_as_2D(struct texture *texture);
static void texture_load_as_cubemap(struct texture *texture);
static void texture_reload(struct texture *texture);
static GLenum format_from_surface(struct SDL_Surface *s);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Generates a plain white, 2x2 texture.
 *
 * @param[out] texture Object receiving the texture.
 */
void texture_2D_default(struct texture *texture)
{
    struct SDL_Surface *def = nullptr;

    def = SDL_CreateRGBSurface(0, 2, 2, 32, 0, 0, 0, 0);
    SDL_FillRect(def, nullptr, 0xffffffff);

    texture->flavor = TEXTURE_FLAVOR_2D;
    texture->specific.image_for_2D = def;
}

/**
 * @brief Loads a texture from a file.
 *
 * @param[out] texture Object receiving the texture.
 * @param[in] path Path to an image.
 */
void texture_2D_file(struct texture *texture, const char *path)
{
    texture->flavor = TEXTURE_FLAVOR_2D;
    texture->specific.image_for_2D = IMG_Load(path);

    texture_reload(texture);
}

/**
 * @brief Loads a texture from a buffer array created with ustd/array.h.
 *
 * @param[out] texture Object receiving the texture.
 * @param[in] image Buffer containing a read image file.
 */
void texture_2D_file_mem(struct texture *texture, const byte *image_array)
{
    texture->flavor = TEXTURE_FLAVOR_2D;
    texture->specific.image_for_2D = IMG_Load_RW(
            SDL_RWFromMem((void *) image_array, array_length(image_array)), 0);

    texture_reload(texture);
}

/**
 * @brief Loads a texture from a file and assigns it to a face of a cubemap
 * texture.
 *
 * @param[out] texture Object receiving the texture.
 * @param[in] face Face of the cubemap.
 * @param[in] image Buffer containing a read image file.
 */
void texture_cubemap_file(struct texture *texture, enum cubemap_face face,
        const char *path)
{
    texture->flavor = TEXTURE_FLAVOR_CUBEMAP;
    texture->specific.images_for_cubemap[face] = IMG_Load(path);

    texture_reload(texture);
}

/**
 * @brief Loads a texture from a buffer array created with ustd/array.h, and
 * assigns it to a face of a cubemap texture.
 *
 * @param[out] texture Object receiving the texture.
 * @param[in] face Face of the cubemap.
 * @param[in] image Buffer containing a read image file.
 */
void texture_cubemap_file_mem(struct texture *texture, enum cubemap_face face,
        const byte *image_array)
{
    texture->flavor = TEXTURE_FLAVOR_CUBEMAP;
    texture->specific.images_for_cubemap[face] = IMG_Load_RW(
            SDL_RWFromMem((void *) image_array, array_length(image_array)), 0);

    texture_reload(texture);
}

/**
 * @brief Destroys a texture, releasing the loaded image(s).
 *
 * @param[inout] texture Destroyed texture.
 */
void texture_delete(struct texture *texture)
{
    size_t nb_textures = 0;

    switch (texture->flavor) {
        case TEXTURE_FLAVOR_2D:
            SDL_FreeSurface(texture->specific.image_for_2D);
            break;
        case TEXTURE_FLAVOR_CUBEMAP:
            nb_textures = COUNT_OF(texture->specific.images_for_cubemap);
            for (size_t i = 0 ; i < nb_textures ; i++) {
                SDL_FreeSurface(texture->specific.images_for_cubemap[i]);
            }
            break;
    }
}

/**
 * @brief Loads a texture to the GPU.
 *
 * @param[inout] texture Loaded texture.
 */
void texture_load(struct texture *texture)
{
    loadable_add_user((struct loadable *) texture);

    if (loadable_needs_loading((struct loadable *) texture)) {
        glGenTextures(1, &texture->gpu_side.name);

        switch (texture->flavor) {
        case TEXTURE_FLAVOR_2D:
            texture_load_as_2D(texture);
            break;
        case TEXTURE_FLAVOR_CUBEMAP:
            texture_load_as_cubemap(texture);
            break;
        }

        texture->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief Unloads a tetxure from the GPU.
 *
 * @param[inout] texture Unloaded texture.
 */
void texture_unload(struct texture *texture)
{
    loadable_remove_user((struct loadable *) texture);

    if (loadable_needs_unloading((struct loadable *) texture)) {

        glDeleteTextures(1, &texture->gpu_side.name);
        texture->gpu_side.name = 0;

        texture->load_state.flags &= ~LOADABLE_FLAG_LOADED;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Loads a texture as if it were holding a single 2D image.
 *
 * @param[inout] texture
 */
static void texture_load_as_2D(struct texture *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->gpu_side.name);
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                texture->specific.image_for_2D->w,
                texture->specific.image_for_2D->h, 0,
                format_from_surface(texture->specific.image_for_2D),
                GL_UNSIGNED_BYTE, texture->specific.image_for_2D->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Loads a texture as if it were holding a set of cubemap images.
 *
 * @param[inout] texture
 */
static void texture_load_as_cubemap(struct texture *texture)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->gpu_side.name);
    {

        for (size_t i = 0 ; i < CUBEMAP_FACES_NUMBER ; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                texture->specific.images_for_cubemap[i]->w,
                texture->specific.images_for_cubemap[i]->h, 0,
                format_from_surface(texture->specific.images_for_cubemap[i]),
                GL_UNSIGNED_BYTE,
                texture->specific.images_for_cubemap[i]->pixels);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/**
 * @brief Approximates the format used by a SDL surface by matching the number
 * of bytes used per pixels to an OpenGL representation that uses the same
 * number.
 *
 * @param[in] s
 * @return GLenum
 */
static GLenum format_from_surface(struct SDL_Surface *s)
{
    switch (s->format->BytesPerPixel) {
        case 1u: return GL_RED;
        case 2u: return GL_RG;
        case 3u: return GL_RGB;
        case 4u: return GL_RGBA;

        default:
            break;
    }
    return GL_NONE;
}

/**
 * @brief Reloads a texture. Useful for image changes.
 *
 * @param[inout] texture
 */
static void texture_reload(struct texture *texture)
{
    if (!(texture->load_state.flags & LOADABLE_FLAG_LOADED)) {
        return;
    }

    texture_unload(texture);
    texture_load(texture);
}
