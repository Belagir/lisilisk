
#include <time.h>

#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(int argc, const char *argv[])
{
    (void) argc;

    srand(42);

    struct application target = application_create(argv[0], 1200, 800);

    struct texture sky_right = { };
    texture_file(&sky_right, "images/skybox/right.jpg");
    struct texture sky_left = { };
    texture_file(&sky_left, "images/skybox/left.jpg");
    struct texture sky_top = { };
    texture_file(&sky_top, "images/skybox/top.jpg");
    struct texture sky_bottom = { };
    texture_file(&sky_bottom, "images/skybox/bottom.jpg");
    struct texture sky_front = { };
    texture_file(&sky_front, "images/skybox/front.jpg");
    struct texture sky_back = { };
    texture_file(&sky_back, "images/skybox/back.jpg");

    // struct texture *skybox[SKYBOX_FACES_NUMBER] = { &sky_right, &sky_left, &sky_top,
            // &sky_bottom, &sky_front, &sky_back, };

    struct shader sky_shader = { };
    shader_frag(&sky_shader, "shaders/3dful_shaders/skybox_frag.glsl");
    shader_vert(&sky_shader, "shaders/3dful_shaders/skybox_vert.glsl");
    shader_link(&sky_shader);

    struct scene scene = { };
    scene_create(&scene);

    // -----------------------------------------------------------------------

    i32 should_quit = 0;
    SDL_Event event = { };
    u32 time = 0;

    scene_load(&scene);

    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        glClearColor(0.4, 0.5, 0.7, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            scene_draw(scene, time);
            // ...
            time += 1;
        }
        SDL_GL_SwapWindow(target.sdl_window);
    }

    scene_delete(&scene);

    shader_delete(&sky_shader);

    texture_delete(&sky_top);
    texture_delete(&sky_left);
    texture_delete(&sky_front);
    texture_delete(&sky_right);
    texture_delete(&sky_back);
    texture_delete(&sky_bottom);

    application_destroy(&target);

    return 0;
}
