
#include "3dful/3dful.h"
#include <time.h>

int main(int argc, const char *argv[])
{
    (void) argc;

    srand(42);

    struct application target = application_create(argv[0], 1200, 800);

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct scene scene = { };
    scene_create(&scene);
    scene_load(&scene);
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    struct shader material_shader = { };
    shader_material_frag(&material_shader, "shaders/user_shaders/material.frag");
    shader_material_vert(&material_shader, "shaders/user_shaders/material.vert");
    shader_link(&material_shader);

    struct texture default_texture = { };
    texture_2D_default(&default_texture);

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct texture shroom_texture = { };
    texture_2D_file(&shroom_texture, "models/mushroom/ShroomBase.png");
    struct texture shroom_spec_texture = { };
    texture_2D_file(&shroom_spec_texture, "models/mushroom/SpecularMap.png");

    struct material shroom_material = { };
    material_texture(&shroom_material, &shroom_texture);
    material_ambient(&shroom_material,  (f32[4]) { 1.0, 1.0, 1.0, }, .15);
    material_ambient_mask(&shroom_material, &shroom_texture);
    material_specular(&shroom_material, (f32[4]) { 1.0, 1.0, 1.0, }, 1.);
    material_specular_mask(&shroom_material, &shroom_spec_texture);
    material_diffuse(&shroom_material,  (f32[4]) { 1.0, 1., 1.0, }, 2.);
    material_diffuse_mask(&shroom_material, &shroom_texture);
    material_shininess(&shroom_material, 64.);

    struct geometry shroom_geometry = { };
    geometry_create(&shroom_geometry);
    geometry_wavobj(&shroom_geometry, "models/mushroom/mushroom.obj");

    struct model shroom = { };
    model_create(&shroom);
    model_geometry(&shroom, &shroom_geometry);
    model_shader(&shroom, &material_shader);
    model_material(&shroom, &shroom_material);
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct texture rock_texture = { };
    texture_2D_file(&rock_texture,
            "models/rock_floating/FloatingRockBase.png");
    struct texture rock_mask = { };
    texture_2D_file(&rock_mask,
            "models/rock_floating/FloatingRockGrassMask.png");

    struct material rock_material = { };
    material_texture(&rock_material, &rock_texture);
    material_ambient(&rock_material,  (f32[4]) { 1.0, 1.0, 1.0, }, .15);
    material_ambient_mask(&rock_material, &rock_texture);
    material_specular(&rock_material, (f32[4]) { 1.0, 1.0, 1.0, }, 1.2);
    material_specular_mask(&rock_material, &rock_mask);
    material_diffuse(&rock_material,  (f32[4]) { 1.0, 1., 1.0, }, 1.5);
    material_diffuse_mask(&rock_material, &default_texture);
    material_shininess(&rock_material, 1.);

    struct geometry rock_geometry = { };
    geometry_create(&rock_geometry);
    geometry_wavobj(&rock_geometry, "models/rock_floating/RockFloating.obj");

    struct model rock = { };
    model_create(&rock);
    model_geometry(&rock, &rock_geometry);
    model_shader(&rock, &material_shader);
    model_material(&rock, &rock_material);
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct shader sky_shader = { };
    shader_frag(&sky_shader, "shaders/3dful_shaders/skybox_frag.glsl");
    shader_vert(&sky_shader, "shaders/3dful_shaders/skybox_vert.glsl");
    shader_link(&sky_shader);

    struct geometry sphere = { };
    geometry_create(&sphere);
    geometry_wavobj(&sphere, "models/sphere.obj");

    struct texture cubemap = { };
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_RIGHT,
            "images/skybox/right.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_LEFT,
            "images/skybox/left.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_TOP,
            "images/skybox/top.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BOTTOM,
            "images/skybox/bottom.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BACK,
            "images/skybox/back.jpg");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_FRONT,
            "images/skybox/front.jpg");

    struct environment env = { };
    environment_geometry(&env, &sphere);
    environment_ambient(&env, (struct light) { {1, 1, 1, 1} });
    environment_shader(&env, &sky_shader);
    environment_skybox(&env, &cubemap);
    environment_fog(&env, (f32[3]) { .4, .6, .8 }, 200.);
    environment_bg(&env, (f32[3]) { .3, .1, .1 });
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct camera cam = { };
    camera_fov(&cam, 40.);
    camera_aspect(&cam, 1200. / 800.);
    camera_limits(&cam, .1, 300.);
    camera_target(&cam, VECTOR3_ORIGIN);
    camera_position(&cam, (struct vector3) { 0., .3, 1.5 });
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    scene_model(&scene, &rock);
    scene_camera(&scene, &cam);
    scene_environment(&scene, &env);
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    handle_t h = 0;
    scene_light_direc(&scene, &h);
    scene_light_direc_color(&scene, h, (f32[4]) { 1, .9, .8, 1 });
    scene_light_direc_orientation(&scene, h, (vector3) { 0, -1, 0 });

    scene_light_direc(&scene, &h);
    scene_light_direc_color(&scene, h, (f32[4]) { .2, .37, .6, 1 });
    scene_light_direc_orientation(&scene, h, (vector3) { 0, 1, -1 });

    model_instantiate(&rock, &h);
    model_instance_transform(&rock, h, MATRIX4_IDENTITY);
    // -----------------------------------------------------------------------

    i32 should_quit = 0;
    SDL_Event event = { };
    u32 time = 0;

    struct quaternion r = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE,
            0.004);
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        camera_position(&cam, vector3_rotate_by_quaternion(cam.pos, r));
        scene_draw(&scene, time);

        time += 1;

        SDL_GL_SwapWindow(target.sdl_window);
    }

    scene_delete(&scene);

    shader_delete(&material_shader);
    geometry_delete(&shroom_geometry);
    geometry_delete(&rock_geometry);
    model_delete(&shroom);
    model_delete(&rock);

    texture_delete(&default_texture);
    texture_delete(&cubemap);
    texture_delete(&shroom_texture);
    texture_delete(&shroom_spec_texture);
    texture_delete(&rock_texture);
    texture_delete(&rock_mask);
    shader_delete(&sky_shader);
    geometry_delete(&sphere);

    application_destroy(&target);

    return 0;
}
