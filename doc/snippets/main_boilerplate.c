
#include <lisilisk.h>
#include <SDL2/SDL.h>

int main(void)
{
    int running = 1;
    SDL_Event event = { 0 };

    lisk_init(0, "assets");

    lisk_show();
    while (running) {
        while (SDL_PollEvent(&event)) {
            running = running && (event.type != SDL_QUIT);
        }

        lisk_draw();
    }
    lisk_deinit();

    return 0;
}
