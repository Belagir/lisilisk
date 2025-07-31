
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

    lisk_model_geometry("rock", "models/stele/Stele.obj");
    lisk_model_base_texture("rock", "models/stele/SteleBase.png");
    lisk_model_emission_color("rock", &(float[4]) { 1, 1, 1, 1 });
    lisk_model_emission_mask("rock", "models/stele/SteleEmission.png");
    lisk_model_show("rock");

    lisk_model_instanciate("rock", &(float[3]) {  0, 0, 0 }, .2);

    lisk_directional_light_add(&(float[3]) { -1, 0, -3 },
            &(float[4]) { .3, .3, .3, 1 });

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
