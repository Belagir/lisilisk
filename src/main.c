
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(void)
{
    srand(42);

    struct application target = application_create("some name", 1200, 800);

    struct shader grass_shader = { };
    shader_frag(&grass_shader, "shaders/geometry_material.frag");
    shader_vert(&grass_shader, "shaders/geometry_grass.vert");
    shader_link(&grass_shader);

    struct shader ground_shader = { };
    shader_frag(&ground_shader, "shaders/geometry_material.frag");
    shader_vert(&ground_shader, "shaders/geometry.vert");
    shader_link(&ground_shader);

    struct geometry grass_geometry = { };
    geometry_create(&grass_geometry);
    geometry_wavobj(&grass_geometry, "models/grass_blade.obj");
    geometry_load(&grass_geometry);

    struct geometry ground_geometry = { };
    geometry_create(&ground_geometry);
    geometry_wavobj(&ground_geometry, "models/plane.obj");
    geometry_load(&ground_geometry);

    struct material grass_material = { };
    material_ambient(&grass_material,  (f32[4]) { .70, .85, .70, 1 });
    material_specular(&grass_material, (f32[4]) { .10, .16, .10, 1 });
    material_diffuse(&grass_material,  (f32[4]) { .30, .50, .30, 1 });
    material_shininess(&grass_material, 4.);
    material_load(&grass_material);

    struct material ground_material = { };
    material_ambient(&ground_material,  (f32[4]) { .30, .25, .20, 1 });
    material_specular(&ground_material, (f32[4]) { .30, .25, .20, 1 });
    material_diffuse(&ground_material,  (f32[4]) { .30, .25, .20, 1 });
    material_shininess(&ground_material, 1.);
    material_load(&ground_material);

    struct object grass = { };
    object_create(&grass);
    object_geometry(&grass, &grass_geometry);
    object_shader(&grass, &grass_shader);
    object_material(&grass, &grass_material);

    f32 x_offset = 0.f;
    f32 y_offset = 0.f;
    f32 scale = 0.f;
    for (f32 x = -50. ; x < 50. ; x += 0.3) {
        for (f32 y = -50. ; y < 50. ; y += 0.3) {
            x_offset = 0. + ((f32) (rand() % 128) / 128.f) * .25;
            y_offset = 0. + ((f32) (rand() % 128) / 128.f) * .25;
            scale    = .5 + ((f32) (rand() % 128) / 128.f) * 1.5;
            object_instantiate(&grass,
                matrix4_translate(
                    matrix4_scale(matrix4_identity(), (vector3) { scale, scale, scale }),
                    (vector3) { x + x_offset, 0, y + y_offset }
                ));
        }
    }
    printf("there are %ld individual blades of grass !\n", grass.tr_instances->length);

    struct object ground = { };
    object_create(&ground);
    object_geometry(&ground, &ground_geometry);
    object_shader(&ground, &ground_shader);
    object_material(&ground, &ground_material);

    object_instantiate(&ground, matrix4_scale(matrix4_identity(), (vector3) { 50., 1., 50. }));

    struct camera camera = { };
    camera_position(&camera, (vector3) { -50, 12, -40 });
    camera_fov(&camera, 60);
    camera_target(&camera, VECTOR3_ORIGIN);
    camera_limits(&camera, 0.1, 300);
    camera_aspect(&camera, 1.5);

    struct light_directional lightdir = { };
    light_color((struct light *) &lightdir, (f32[4]) { .8, .6, .6, 1 });
    light_directional_direction(&lightdir, (vector3) { 1, -.2, 1 });

    struct light_point lightpoint = { };
    light_color((struct light *) &lightpoint, (f32[4]) { .7, .6, .2, 1 });
    light_position(&lightpoint, (vector3) { 0, 2, 0 });
    light_point_linear(&lightpoint,    1.0);
    light_point_constant(&lightpoint,  0.09);
    light_point_quadratic(&lightpoint, 0.032);

    struct scene scene = { };
    scene_create(&scene);
    scene_light_ambient(&scene, (struct light) { .color = { .3, .3, .3, 1 } });
    scene_light_direc(&scene, lightdir);
    scene_light_point(&scene, lightpoint);
    scene_camera(&scene, camera);
    scene_object(&scene, ground);
    scene_object(&scene, grass);
    scene_load(&scene);

    i32 should_quit = 0;
    SDL_Event event = { };
    struct quaternion cam_rotat = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, .002);
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

        SDL_Delay(10);
    }

    scene_unload(&scene);
    geometry_unload(&grass_geometry);
    geometry_unload(&ground_geometry);
    material_unload(&grass_material);
    material_unload(&ground_material);

    scene_delete(&scene);
    geometry_delete(&grass_geometry);
    geometry_delete(&ground_geometry);
    object_delete(&grass);
    object_delete(&ground);
    shader_delete(&grass_shader);
    shader_delete(&ground_shader);

    application_destroy(&target);

    return 0;
}
