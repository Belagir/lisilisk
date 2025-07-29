/**
 * @file lisilisk.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LISILISK_H__
#define LISILISK_H__

#include <stdint.h>

/** Invalid handle is just full of zeroes. */
#define LISK_HANDLE_NONE ((lisk_handle_t) 0)

/** Handle type to hold basic information and have it be a scalar. */
typedef uint32_t lisk_handle_t;

// Sets the engine ready to be used.
void lisk_init(void);
// Make the engine shut down and release all memory.
void lisk_deinit(void);

// Sets the size of the window, in pixels.
void lisk_resize(
        uint16_t width,
        uint16_t height);
// Sets the title of the window.
void lisk_rename(
        const char *window_name);

// Registers a model to the engine.
// The model is getting a default shader ans material.
void lisk_model(
        const char *name,
        const char *obj_file);

// Instanciate a model at some point in the world.
lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3]);

// Removes a model instance from the world.
void lisk_model_instance_remove(
        const char *model_name,
        lisk_handle_t instance);

// Changes the ambient ight setting of the environment.
void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength);

// Shows the window and the scene.
void lisk_show(void);

#endif
