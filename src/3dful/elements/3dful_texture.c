
#include "3dful_core.h"

#include <ustd/array.h>

/**
 * @brief
 *
 * @param texture
 */
void texture_default(struct texture *texture)
{
    struct SDL_Surface *def = nullptr;

    def = SDL_CreateRGBSurface(0, 2, 2, 32, 0, 0, 0, 0);
    SDL_FillRect(def, nullptr, 0xffffffff);

    texture->image = def;
}

/**
 * @brief
 *
 * @param texture
 * @param path
 */
void texture_file(struct texture *texture, const char *path)
{
    texture->image = IMG_Load(path);
}

/**
 * @brief
 *
 * @param texture
 * @param image
 */
void texture_file_mem(struct texture *texture, const byte *image_array)
{
    texture->image = IMG_Load_RW(SDL_RWFromMem((void *) image_array, array_length(image_array)), 0);
}

/**
 * @brief
 *
 * @param texture
 */
void texture_delete(struct texture *texture)
{
    SDL_FreeSurface(texture->image);
}

/**
 * @brief
 *
 * @param texture
 */
void texture_load(struct texture *texture)
{
    loadable_add_user((struct loadable *) texture);

    if (loadable_needs_loading((struct loadable *) texture)) {
        glGenTextures(1, &texture->gpu_side.name);

        glBindTexture(GL_TEXTURE_2D, texture->gpu_side.name);
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->image->w, texture->image->h,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, texture->image->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        texture->load_state.flags |= LOADABLE_FLAG_LOADED;
    }
}

/**
 * @brief
 *
 * @param texture
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

