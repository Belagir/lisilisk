
#include <SDL2/SDL.h>

#include <ustd/math3d.h>

#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init(NULL, "assets_scene");

    lisk_model_geometry("bubble", "assets_scene/frog/MiniBG.obj");
    lisk_model_base_texture("bubble", "assets_scene/frog/MiniBGBase.png");
    lisk_model_frontface_culling("bubble");
    lisk_model_ambient_color("bubble", &(float[4]) { 1, 1, 1, 1 });
    lisk_model_diffuse_color("bubble", &(float[4]) { 0, 0, 0, 0 });
    lisk_model_specular_color("bubble", &(float[4]) { 0, 0, 0, 0 }, 1);
    lisk_model_show("bubble");

    lisk_model_geometry("rock", "assets_scene/rock_floating/RockFloating.obj");
    lisk_model_base_texture("rock", "assets_scene/rock_floating/FloatingRockBase.png");
    lisk_model_specular_mask("rock", "assets_scene/rock_floating/FloatingRockGrassMask.png");
    lisk_model_show("rock");

    lisk_model_geometry("stele", "assets_scene/stele/Stele.obj");
    lisk_model_base_texture("stele", "assets_scene/stele/SteleBase.png");
    lisk_model_emission_mask("stele", "assets_scene/stele/SteleEmission.png");
    lisk_model_emission_color("stele", &(float[4]) { .4, .8, 1, 1 });
    lisk_model_show("stele");

    lisk_model_geometry("shroom", "assets_scene/mushroom/mushroom.obj");
    lisk_model_base_texture("shroom", "assets_scene/mushroom/ShroomBase.png");
    lisk_model_show("shroom");

    lisk_model_geometry("screen", "assets_scene/frog/FrogScreen.obj");
    lisk_model_show("screen");

    lisk_model_geometry("frog", "assets_scene/frog/Frog.obj");
    lisk_model_base_texture("frog", "assets_scene/frog/FrogBase.png");
    lisk_model_diffuse_color("frog", &(float[4]) { 1.5, 1.5, 1.5, 1 });
    lisk_model_show("frog");

    lisk_handle_t bubble = lisk_model_instanciate("bubble",  &(float[3]) { 0, 0, 0 }, .8);
    lisk_handle_t rock   = lisk_model_instanciate("rock",    &(float[3]) { 0, 0, 0 }, .5);
    lisk_handle_t stele  = lisk_model_instanciate("stele",   &(float[3]) { 0.05, 0, -.1 }, .12);
    lisk_handle_t shroom1 = lisk_model_instanciate("shroom", &(float[3]) { -0.06, 0, .1 }, .04);
    lisk_handle_t shroom2 = lisk_model_instanciate("shroom", &(float[3]) {  0.12, 0, .15 }, .02);

    lisk_handle_t screen = lisk_model_instanciate("screen", &(float[3]) { 0, .0, 2. }, 0.9);
    lisk_instance_set_rotation(screen, &(float[3]) { 0, 1, 0 }, PI/2);

    lisk_model_instanciate("frog", &(float[3]) { 1, -1, 6. }, 6);

    lisk_handle_t l_stele = lisk_point_light_add(&(float[3]) { 0, .1, 0 },
            &(float[4]) { .5, .6, .8, 1 }, 1., .8, .5);

    lisk_point_light_add(&(float[3]) { -.08, .2, .12 },
            &(float[4]) { .6, .6, .6, 1 }, 1., .8, .8);

    lisk_point_light_add(&(float[3]) { 1.0, -.2, 4. },
            &(float[4]) { .4, .7, 1, 1 }, 1., .2, .00);

    lisk_instance_set_position(lisk_camera(), &(float[3]) {  -2.8, .8, -3.5 });
    lisk_instance_camera_set_fov(lisk_camera(), 12.);
    lisk_instance_camera_set_target(lisk_camera(), &(float[3]) { 0.2, .1, 1.5 });

    lisk_ambient_light_set(1, 1, 1, 1);
    lisk_bg_color_set(&(float[3]) { 0, 0, 0});
    lisk_show();

    SDL_Event event = { };
    int quit = 0;

    struct quaternion q = quaternion_identity();
    struct quaternion r = quaternion_from_axis_and_angle(VECTOR3_Y_POSITIVE, 0.01);
    struct vector3 tmp = { };

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            quit = quit || (event.type == SDL_QUIT);
        }

        tmp = vector3_rotate_by_quaternion((struct vector3) { 0, 0, 0 }, q);
        lisk_instance_set_position(bubble, (float(*)[3]) &tmp);
        lisk_instance_set_rotation_quaternion(bubble, (float(*)[4]) &q);
        lisk_instance_set_position(rock, (float(*)[3]) &tmp);
        lisk_instance_set_rotation_quaternion(rock, (float(*)[4]) &q);

        tmp = vector3_rotate_by_quaternion((struct vector3) { 0.05, 0, -.1 },
                quaternion_conjugate(q));
        lisk_instance_set_position(stele, (float(*)[3]) &tmp);
        lisk_instance_set_rotation_quaternion(stele, (float(*)[4]) &q);

        tmp = vector3_rotate_by_quaternion((struct vector3) { -0.06, 0, .1 },
                quaternion_conjugate(q));
        lisk_instance_set_position(shroom1, (float(*)[3]) &tmp);
        lisk_instance_set_rotation_quaternion(shroom1, (float(*)[4]) &q);

        tmp = vector3_rotate_by_quaternion((struct vector3) {  0.12, 0, .15 },
                quaternion_conjugate(q));
        lisk_instance_set_position(shroom2, (float(*)[3]) &tmp);
        lisk_instance_set_rotation_quaternion(shroom2, (float(*)[4]) &q);

        tmp = vector3_rotate_by_quaternion((struct vector3) { -.08, .2, .12 },
                quaternion_conjugate(q));
        lisk_instance_set_position(l_stele, (float(*)[3]) &tmp);

        q = quaternion_multiply(q, r);

        lisk_draw();
    }

    lisk_deinit();
}
