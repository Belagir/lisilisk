
#ifndef LISILISK_H__
#define LISILISK_H__

#include <stdint.h>

#define LISK_HANDLE_NONE ((lisk_handle_t) 0)

typedef uint32_t lisk_handle_t;

void lisk_init(void);
void lisk_deinit(void);

void lisk_resize(
        uint16_t width,
        uint16_t height);
void lisk_rename(
        const char *window_name);

void lisk_model(
        const char *name,
        const char *obj_file);

lisk_handle_t lisk_model_instanciate(
        const char *model_name,
        float (*pos)[3],
        float scale);

void lisk_model_instance_remove(
        const char *model_name,
        lisk_handle_t instance);

void lisk_ambient_light_set(
        float r,
        float g,
        float b,
        float strength);

void lisk_show(void);

#endif
