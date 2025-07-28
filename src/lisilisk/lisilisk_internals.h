
#ifndef LISILISK_INTERNALS_H__
#define LISILISK_INTERNALS_H__

#include <lisilisk.h>

#include "../3dful/3dful.h"

void lisilisk_default_environment(struct environment *env);
void lisilisk_default_camera(struct camera *camera, struct SDL_Window *window);

#endif
