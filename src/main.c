
#include <lisilisk.h>
#include <time.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

#if WONDERING_ABOUT_THE_INTERFACE

    lisk_init();
    lisk_resize(1200, 800);
    lisk_rename("Lisilisk test");

    lisk_model_instanciate("models/mushroom/mushroom.obj",
        &(float[3]) { 0.f, 0.f, 0.f }, 1.f);

    lisk_ambient_light_set(1, .9, .8, 1.);

    lisk_show();

    lisk_deinit();
#endif

    return 0;
}
