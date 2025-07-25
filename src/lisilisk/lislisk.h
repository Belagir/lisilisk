
#ifndef LISILISK_H__
#define LISILISK_H__

#include <stdint.h>
#include <stddef.h>

typedef uint32_t lisk_handle_t;

void lisk_init(void);
void lisk_deinit(void);

lisk_handle_t lisk_model(
        const char *obj_file);

lisk_handle_t lisk_model_instanciate(
        lisk_handle_t model, 
        float pos[3], 
        float scale);

void lisk_model_instance_remove(
        lisk_handle_t instance);

void lisk_ambient_light(
        float r, 
        float g, 
        float b, 
        float strength);

void lisk_run(void);

#endif
