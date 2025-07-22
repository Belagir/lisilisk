
#include <time.h>

#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(int argc, const char *argv[])
{
    (void) argc;

    srand(42);

    struct application target = application_create(argv[0], 1200, 800);

    struct shader material_shader = { };
    shader_material_frag(&material_shader, "shaders/user_shaders/material.frag");
    shader_material_vert(&material_shader, "shaders/user_shaders/material.vert");
    shader_link(&material_shader);

    struct texture default_texture = { };
    texture_2D_default(&default_texture);

    struct material saucer_material = { };
    material_ambient(&saucer_material,  (f32[4]) { .10, .10, .10, }, 1);
    material_ambient_mask(&saucer_material, &default_texture);
    material_specular(&saucer_material, (f32[4]) { .95, .95, 1.0, }, 1);
    material_specular_mask(&saucer_material, &default_texture);
    material_diffuse(&saucer_material,  (f32[4]) { .32, .32, .32, }, 0);
    material_diffuse_mask(&saucer_material, &default_texture);
    material_shininess(&saucer_material, 4.);

    struct geometry saucer_geometry = { };
    geometry_create(&saucer_geometry);
    geometry_wavobj(&saucer_geometry, "models/saucer.obj");

    struct model saucer = { };
    model_create(&saucer);
    model_geometry(&saucer, &saucer_geometry);
    model_shader(&saucer, &material_shader);
    model_material(&saucer, &saucer_material);
    model_instantiate(&saucer, MATRIX4_IDENTITY);
    model_instantiate(&saucer, matrix4_translate(MATRIX4_IDENTITY, (vector3) { 0, 2, -180. }));

    struct shader sky_shader = { };
    shader_frag(&sky_shader, "shaders/3dful_shaders/skybox_frag.glsl");
    shader_vert(&sky_shader, "shaders/3dful_shaders/skybox_vert.glsl");
    shader_link(&sky_shader);

    struct camera cam = { };
    camera_fov(&cam, 40.);
    camera_aspect(&cam, 1200. / 800.);
    camera_limits(&cam, .1, 300.);
    camera_target(&cam, VECTOR3_ORIGIN);
    camera_position(&cam, (struct vector3) { 1, 1.75, 15 });

    struct geometry cube = { };
    geometry_create(&cube);
    geometry_wavobj(&cube, "models/cube.obj");

    struct texture cubemap = { };
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_RIGHT,  "images/skybox/right.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_LEFT,   "images/skybox/left.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_TOP,    "images/skybox/top.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BOTTOM, "images/skybox/bottom.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BACK,   "images/skybox/back.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_FRONT,  "images/skybox/front.jpg");

    struct environment env = { };
    environment_cube(&env, &cube);
    environment_ambient(&env, (struct light) { {1, 1, 1, 1} });
    environment_shader(&env, &sky_shader);
    environment_skybox(&env, &cubemap);
    environment_fog(&env, (f32[3]) { .4, .6, .8 }, 200.);
    environment_bg(&env, (f32[3]) { .3, .1, .1 });

    struct scene scene = { };
    scene_create(&scene);
    scene_camera(&scene, &cam);
    scene_environment(&scene, &env);
    scene_light_direc(&scene, (struct light_directional) { .color = { 1., .9, .8, 1. },
            .direction = (struct vector3) { 0, -1, .3 } });
    scene_model(&scene, &saucer);

    // -----------------------------------------------------------------------
    scene_load(&scene);
    // -----------------------------------------------------------------------

    i32 should_quit = 0;
    SDL_Event event = { };
    u32 time = 0;

    struct quaternion r = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, 0.02);
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        camera_position(&cam, vector3_rotate_by_quaternion(cam.pos, r));
        scene_draw(&scene, time);

        SDL_GL_SwapWindow(target.sdl_window);
    }

    scene_delete(&scene);

    shader_delete(&material_shader);
    geometry_delete(&saucer_geometry);
    model_delete(&saucer);
    texture_delete(&default_texture);

    shader_delete(&sky_shader);
    geometry_delete(&cube);

    application_destroy(&target);

    return 0;
}
