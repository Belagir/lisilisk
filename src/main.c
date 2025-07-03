
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"

void drawscene(struct application target)
{
    struct shader shader = { };
    struct geometry geometry = { };
    struct object object = { };

    shader_frag(&shader, "shaders/dummy.frag");
    shader_vert(&shader, "shaders/dummy.vert");
    shader_link(&shader);

    geometry_wavobj(&geometry, "models/diamond.obj");

    object_transform(&object, matrix_translate(matrix4_identity(), (vector3_t) { .5, .5, 0 }));
    object_geometry(&object, &geometry);
    object_shader(&object, &shader);
    object_load(&object);

    int should_quit = 0;
    SDL_Event event = { };
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            object_draw(object);
        }
        SDL_GL_SwapWindow(target.sdl_window);

        SDL_Delay(100);
    }

    object_unload(&object);
    shader_delete(&shader);
    geometry_delete(&geometry);
}

int main(void)
{
    struct application target = application_create("some name", 800, 800);

    drawscene(target);

    application_destroy(&target);

    return 0;
}
