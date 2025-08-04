
#include <SDL2/SDL.h>

#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init("Nowhere", "assets/");

    lisk_skybox_set(&(const char *[6]) {
            "assets/images/star_shower/right.png",
            "assets/images/star_shower/left.png",
            "assets/images/star_shower/top.png",
            "assets/images/star_shower/bottom.png",
            "assets/images/star_shower/front.png",
            "assets/images/star_shower/back.png",
    });

    lisk_model_geometry("shroom", "assets/models/mushroom/mushroom.obj");
    lisk_model_base_texture("shroom", "assets/models/mushroom/ShroomBase.png");
    lisk_model_show("shroom");

    lisk_model_instanciate("shroom", &(float[3]) {  0, 0, 0 }, .2);

    lisk_directional_light_add(&(float[3]) { .5, 0, -1 },
            &(float[4]) { .3, .3, .3, 1 });
    lisk_directional_light_add(&(float[3]) { 0, -.5, 1 },
            &(float[4]) { .3, .3, 0, 1 });

    lisk_instance_camera_set_fov(lisk_camera(), 65.);

    lisk_show();

    SDL_Event event = { };
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            quit = quit || (event.type == SDL_QUIT);
        }

        lisk_draw();
    }

    lisk_deinit();
}
