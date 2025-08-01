
#include <SDL2/SDL.h>

#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init();
    lisk_rename("in nowhere");

    lisk_skybox_set(&(const char *[6]) {
            "images/star_shower/right.png",
            "images/star_shower/left.png",
            "images/star_shower/top.png",
            "images/star_shower/bottom.png",
            "images/star_shower/front.png",
            "images/star_shower/back.png",
    });

    lisk_model_geometry("terrain", "models/test_terrain.obj");
    lisk_model_show("terrain");

    lisk_model_instanciate("terrain", &(float[3]) {  0, 10, 0 }, 10);

    lisk_directional_light_add(&(float[3]) { 0, 0, -1 },
            &(float[4]) { .3, .3, .3, 1 });
    lisk_directional_light_add(&(float[3]) { 0, .5, 1 },
            &(float[4]) { .3, .3, 0, 1 });

    lisk_instance_camera_set_fov(lisk_camera(), 65.);

    lisk_show();

    SDL_Event event = { };
    int quit = 0;
    float cam_r = 0;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            quit = quit || (event.type == SDL_QUIT);
        }

        lisk_instance_set_rotation(lisk_camera(),
                &(float[3]) { 0, 1, 0 }, cam_r);
        cam_r = fmodf(cam_r+.02, 6.3);

        lisk_draw();
    }

    lisk_deinit();
}
