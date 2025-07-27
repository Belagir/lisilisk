
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
    shader_material_frag(&material_shader,
            "shaders/user_shaders/material.frag");
    shader_material_vert(&material_shader,
            "shaders/user_shaders/material.vert");
    shader_link(&material_shader);

    struct texture default_texture = { };
    texture_2D_default(&default_texture);

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    struct texture shroom_texture = { };
    texture_2D_file(&shroom_texture, "models/mushroom/ShroomBase.png");

    struct material shroom_material = { };
    material_texture(&shroom_material, &shroom_texture);
    material_ambient(&shroom_material,  (f32[3]) { 1.0, 1.0, 1.0, }, .15);
    material_ambient_mask(&shroom_material, &default_texture);
    material_specular(&shroom_material, (f32[3]) { 1.0, 1.0, 1.0, }, 1.);
    material_specular_mask(&shroom_material, &default_texture);
    material_diffuse(&shroom_material,  (f32[3]) { 1.0, 1.0, 1.0, }, 1.3);
    material_diffuse_mask(&shroom_material, &default_texture);
    material_shininess(&shroom_material, 1.);

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
    material_ambient(&rock_material,  (f32[3]) { 1.0, 1.0, 1.0, }, .15);
    material_ambient_mask(&rock_material, &default_texture);
    material_specular(&rock_material, (f32[3]) { 1.0, 1.0, 1.0, }, 1.2);
    material_specular_mask(&rock_material, &rock_mask);
    material_diffuse(&rock_material,  (f32[3]) { 1.0, 1., 1.0, }, 1.5);
    material_diffuse_mask(&rock_material, &default_texture);
    material_shininess(&rock_material, 64.);

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
    struct texture stele_texture = { };
    texture_2D_file(&stele_texture,
            "models/stele/SteleBase.png");
    struct texture stele_mask = { };
    texture_2D_file(&stele_mask,
            "models/stele/SteleEmission.png");

    struct material stele_material = { };
    material_texture(&stele_material, &stele_texture);
    material_ambient(&stele_material,  (f32[3]) { 1, 1, 1 }, .15);
    material_ambient_mask(&stele_material, &default_texture);
    material_specular(&stele_material, (f32[3]) { 1, 1, 1 }, 1);
    material_specular_mask(&stele_material, &default_texture);
    material_diffuse(&stele_material,  (f32[3]) { 1, 1, 1 }, 1);
    material_diffuse_mask(&stele_material, &default_texture);
    material_shininess(&stele_material, 4.);
    material_emissive(&stele_material, (f32[3]) { .4, .6, 1 }, .8);
    material_emissive_mask(&stele_material, &stele_mask);

    struct geometry stele_geometry = { };
    geometry_create(&stele_geometry);
    geometry_wavobj(&stele_geometry, "models/stele/Stele.obj");

    struct model stele = { };
    model_create(&stele);
    model_geometry(&stele, &stele_geometry);
    model_shader(&stele, &material_shader);
    model_material(&stele, &stele_material);
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
            "images/star_shower/right.png");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_LEFT,
            "images/star_shower/left.png");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_TOP,
            "images/star_shower/top.png");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BOTTOM,
            "images/star_shower/bottom.png");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_BACK,
            "images/star_shower/back.png");
    texture_cubemap_file(&cubemap, CUBEMAP_FACE_FRONT,
            "images/star_shower/front.png");

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
    camera_position(&cam, (struct vector3) { 0., .4, 1 });
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    scene_model(&scene, &rock);
    scene_model(&scene, &stele);
    scene_model(&scene, &shroom);
    scene_camera(&scene, &cam);
    scene_environment(&scene, &env);
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    handle_t h = 0;
    model_instantiate(&rock, &h);
    model_instance_scale(&rock, h, 1.);
    model_instance_rotation(&rock, h, quaternion_identity());

    model_instantiate(&stele, &h);
    model_instance_scale(&stele, h, .15);
    model_instance_position(&stele, h, (vector3) { 0, .03, -.20 });
    model_instance_rotation(&stele, h,
            quaternion_from_axis_and_angle(VECTOR3_Z_POSITIVE, PI/32));

    scene_light_point(&scene, &h);
    scene_light_point_color(&scene, h, (f32[4]) { 0.2, 0.5, 1, 1 });
    scene_light_point_position(&scene, h, (vector3) { -0.08, .2, 0. });
    scene_light_point_attenuation(&scene, h, 1, 1, 1);

    model_instantiate(&shroom, &h);
    model_instance_scale(&shroom, h, .05);
    model_instance_position(&shroom, h, (vector3) { .06, 0, .10 });
    model_instance_rotation(&shroom, h,
            quaternion_from_axis_and_angle(VECTOR3_Z_POSITIVE, PI/24));
    model_instantiate(&shroom, &h);
    model_instance_scale(&shroom, h, .03);
    model_instance_position(&shroom, h, (vector3) { -.2, .01, .3 });
    model_instance_rotation(&shroom, h,
            quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, PI));
    model_instantiate(&shroom, &h);
    model_instance_scale(&shroom, h, .035);
    model_instance_position(&shroom, h, (vector3) { -.25, .01, .05 });
    model_instance_rotation(&shroom, h,
            quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, PI/4));

    scene_light_direc(&scene, &h);
    scene_light_direc_color(&scene, h, (f32[4]) { .1, .05 ,.1, 1. });
    scene_light_direc_orientation(&scene, h, (vector3) { -.2, .3, 1 });

    scene_light_direc(&scene, &h);
    scene_light_direc_color(&scene, h, (f32[4]) { .25, .1 ,.25, 1. });
    scene_light_direc_orientation(&scene, h, (vector3) { 0, 1, .3 });
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
    shader_delete(&sky_shader);

    geometry_delete(&shroom_geometry);
    geometry_delete(&rock_geometry);
    geometry_delete(&stele_geometry);

    model_delete(&shroom);
    model_delete(&rock);
    model_delete(&stele);

    texture_delete(&default_texture);
    texture_delete(&cubemap);

    texture_delete(&shroom_texture);

    texture_delete(&stele_texture);
    texture_delete(&stele_mask);

    texture_delete(&rock_texture);
    texture_delete(&rock_mask);

    geometry_delete(&sphere);

    application_destroy(&target);

    return 0;
}
