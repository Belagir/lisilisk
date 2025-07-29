
#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init();
    lisk_rename("in nowhere");

    lisk_model_instanciate("shroom", &(float[3]) {  0, 0, 0 });
    lisk_model_instanciate("shroom", &(float[3]) {  1, 0, 0 });
    lisk_model_instanciate("shroom", &(float[3]) { -1, 0, 0 });
    lisk_model_geometry("shroom", "models/mushroom/mushroom.obj");

    lisk_model_show("shroom");

    lisk_show();

    lisk_deinit();
}
