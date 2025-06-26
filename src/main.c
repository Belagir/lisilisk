
#include "3d/opengl_scenes.h"

int main(void)
{
    struct ogl_target target = ogl_target_create("some name", 1200,800);

    ogl_target_destroy(&target);

    return 0;
}
