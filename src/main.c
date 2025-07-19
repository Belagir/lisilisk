
#include <time.h>

#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(int argc, const char *argv[])
{
    (void) argc;

    srand(42);

    struct application target = application_create(argv[0], 1200, 800);

    struct shader standard_shader = { };
    shader_frag(&standard_shader, "shaders/user_shaders/material.frag");
    shader_vert(&standard_shader, "shaders/user_shaders/material.vert");
    shader_link(&standard_shader);

    struct geometry saucer_geometry = { };
    geometry_create(&saucer_geometry);
    geometry_wavobj(&saucer_geometry, "models/cube.obj");

    struct texture default_texture = { };
    texture_default(&default_texture);

    struct texture numbers = { };
    texture_file(&numbers, "images/numbers.png");

    struct material saucer_material = { };
    material_texture(&saucer_material, &default_texture);
    material_ambient(&saucer_material,  (f32[3]) { 1.0, 1.0, 1.0 }, 0.1);
    material_ambient_mask(&saucer_material, &default_texture);
    material_specular(&saucer_material, (f32[3]) { 0.9, 0.9, 1.0 }, 0.4);
    material_specular_mask(&saucer_material, &default_texture);
    material_shininess(&saucer_material, 4.);
    material_diffuse(&saucer_material,  (f32[3]) { 1.0, 0.7, 0.7 }, 0.8);
    material_diffuse_mask(&saucer_material, &default_texture);
    material_emissive(&saucer_material,  (f32[3]) { 1.0, 0.0, 0.0 }, 1.0);
    material_emissive_mask(&saucer_material, &numbers);

    struct model saucer = { };
    model_create(&saucer);
    model_geometry(&saucer, &saucer_geometry);
    model_shader(&saucer, &standard_shader);
    model_material(&saucer, &saucer_material);
    model_instantiate(&saucer, matrix4_translate(matrix4_identity(), (vector3) { 2., 2., 0. }));
    model_instantiate(&saucer, matrix4_translate(matrix4_identity(), (vector3) { 1.5, -1., .4 }));

    struct camera camera = { };
    camera_position(&camera, (vector3) { -10, 4, -7 });
    camera_fov(&camera, 60);
    camera_target(&camera, VECTOR3_ORIGIN);
    camera_limits(&camera, 0.1, 300);
    camera_aspect(&camera, 1.5);

    struct light_directional lightdir = { };
    light_color((struct light *) &lightdir, (f32[4]) { .7, .7, .7, 1 });
    light_directional_direction(&lightdir, (vector3) { 1, -.2, 1 });

    struct light_point lightpoint = { };
    light_color((struct light *) &lightpoint, (f32[4]) { 1., 1., 1., 1 });
    light_position(&lightpoint, (vector3) { 1, 15, 0 });
    light_point_linear(&lightpoint,    1.0);
    light_point_constant(&lightpoint,  0.4);
    light_point_quadratic(&lightpoint, 0.15);

    struct scene scene = { };
    scene_create(&scene);
    scene_light_ambient(&scene, (struct light) { .color = { .3, .3, .3, 1 } });
    scene_light_direc(&scene, lightdir);
    scene_light_point(&scene, lightpoint);
    scene_camera(&scene, camera);
    scene_model(&scene, &saucer);
    scene_load(&scene);

    i32 should_quit = 0;
    SDL_Event event = { };

    struct quaternion cam_rotat = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, .008);
    u32 time = 0;

    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        glClearColor(0.4, 0.5, 0.7, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            scene_draw(scene, time);
            time += 1;
        }
        SDL_GL_SwapWindow(target.sdl_window);

        camera_position(&scene.camera,
                vector3_rotate_by_quaternion(scene.camera.pos, cam_rotat));
    }

    scene_unload(&scene);

    scene_delete(&scene);
    geometry_delete(&saucer_geometry);
    model_delete(&saucer);
    texture_delete(&default_texture);
    texture_delete(&numbers);
    shader_delete(&standard_shader);

    application_destroy(&target);

    return 0;
}
