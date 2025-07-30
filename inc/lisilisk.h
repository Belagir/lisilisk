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
typedef uint64_t lisk_handle_t;

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

// Makes it mossible for a model's instances to be rendered to the scene.
void lisk_model_show(
        const char *name);

// Changes the geometry of a model.
void lisk_model_geometry(
        const char *name,
        const char *obj_file);

// Instanciate a model at some point in the world.
lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3]);

// Removes a model instance from the world.
void lisk_instance_remove(
        lisk_handle_t instance);

// Changes the scale of an instance.
void lisk_instance_set_scale(
        lisk_handle_t instance,
        float scale);

// Changes the scale of an instance.
void lisk_instance_set_position(
        lisk_handle_t instance,
        float (*pos)[3]);

// Turns an instance around an axis.
void lisk_instance_rotate(
        lisk_handle_t instance,
        float (*axis)[3],
        float angle_rad);

// Changes the ambient ight setting of the environment.
void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength);

// Shows the window and the scene.
void lisk_show(void);

#endif
