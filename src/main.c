
#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init();
    lisk_rename("in nowhere");

    lisk_model("shroom", "models/mushroom/mushroom.obj");

    lisk_show();

    lisk_deinit();
}
