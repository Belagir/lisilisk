
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

    struct texture *skybox[CUBEMAP_FACES_NUMBER] = { &sky_right, &sky_left, &sky_top,
            &sky_bottom, &sky_front, &sky_back, };

    struct shader sky_shader = { };
    shader_frag(&sky_shader, "shaders/3dful_shaders/skybox_frag.glsl");
    shader_vert(&sky_shader, "shaders/3dful_shaders/skybox_vert.glsl");
    shader_link(&sky_shader);

    struct camera cam = { };
    camera_fov(&cam, 60.);
    camera_aspect(&cam, (f32) (1200/800));
    camera_limits(&cam, .1, 300.);
    camera_target(&cam, VECTOR3_ORIGIN);
    camera_position(&cam, (struct vector3) { 3, 1, 3 });

    struct geometry cube = { };
    geometry_create(&cube);
    geometry_wavobj(&cube, "models/cube.obj");

    struct environment env = { };
    environment_cube(&env, &cube);
    environment_ambient(&env, (struct light) { {1, 1, 1, 1} });
    environment_shader(&env, &sky_shader);
    environment_skybox(&env, &skybox);

    struct scene scene = { };
    scene_create(&scene);
    scene_camera(&scene, cam);
    scene_environment(&scene, &env);

    // -----------------------------------------------------------------------
    scene_load(&scene);
    // -----------------------------------------------------------------------

    i32 should_quit = 0;
    SDL_Event event = { };
    u32 time = 0;

    struct quaternion cam_rotat = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, 0.005);

    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        scene_draw(scene, time);

        camera_position(&scene.camera, vector3_rotate_by_quaternion(scene.camera.pos, cam_rotat));
        time += 1;
        SDL_GL_SwapWindow(target.sdl_window);
    }

    scene_delete(&scene);

    shader_delete(&sky_shader);
    geometry_delete(&cube);

    texture_delete(&sky_top);
    texture_delete(&sky_left);
    texture_delete(&sky_front);
    texture_delete(&sky_right);
    texture_delete(&sky_back);
    texture_delete(&sky_bottom);

    application_destroy(&target);

    return 0;
}
