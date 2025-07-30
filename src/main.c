
#include <lisilisk.h>

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    lisk_init();
    lisk_rename("in nowhere");

    lisk_handle_t shroom1 = lisk_model_instanciate("shroom",
            &(float[3]) { -1, 0, 0 });
    lisk_handle_t shroom2 = lisk_model_instanciate("shroom",
            &(float[3]) {  0, 0, 0 });
    lisk_handle_t shroom3 = lisk_model_instanciate("shroom",
            &(float[3]) {  1, 0, 0 });

    lisk_instance_set_scale(shroom1, .1);
    lisk_instance_set_scale(shroom2, .2);
    lisk_instance_set_scale(shroom3, .3);

    lisk_instance_rotate(shroom1, &(float[3]) { 0, 0, 1 }, 1.6);
    lisk_instance_rotate(shroom2, &(float[3]) { 0, 1, 0 }, 1.6);
    lisk_instance_rotate(shroom3, &(float[3]) { 1, 0, 0 }, 1.6);

    lisk_model_geometry("shroom", "models/mushroom/mushroom.obj");
    lisk_model_show("shroom");

    lisk_show();

    lisk_deinit();
}
