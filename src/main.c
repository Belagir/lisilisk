
#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init();
    lisk_rename("in nowhere");

    lisk_model("shroom", "models/mushroom/mushroom.obj");

    lisk_handle_t h = lisk_model_instanciate("shroom",
            &(float[3]) {  0, 0, 0 }, .2);

    lisk_model_instanciate("shroom", &(float[3]) {  1, 0, 0 }, .2);
    lisk_model_instanciate("shroom", &(float[3]) { -1, 0, 0 }, .2);

    lisk_model_instance_remove("shroom", h);


    lisk_show();

    lisk_deinit();
}
