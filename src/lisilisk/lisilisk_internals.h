/**
 * @file lisilisk_internals.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LISILISK_INTERNALS_H__
#define LISILISK_INTERNALS_H__

#include <lisilisk.h>

#include "../3dful/3dful.h"

void lisilisk_default_environment(
        struct environment *env);
void lisilisk_default_camera(
        struct camera *camera,
        struct SDL_Window *window);

void lisilisk_create_default_material_shader(
        struct shader *shader);
void lisilisk_create_default_texture(
        struct texture *texture);
void lisilisk_default_material(
        struct material *material,
        struct texture *texture);

#endif
