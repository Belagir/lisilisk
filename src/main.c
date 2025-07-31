
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

    lisk_instance_set_rotation(shroom1, &(float[3]) { 0, 0, 1 }, 1.6);
    lisk_instance_set_rotation(shroom2, &(float[3]) { 0, 1, 0 }, 1.6);
    lisk_instance_set_rotation(shroom3, &(float[3]) { 1, 0, 0 }, 1.6);

    lisk_model_geometry("shroom", "models/mushroom/mushroom.obj");
    lisk_model_ambient_color("shroom",  &(float[4]) { 1, 1, 1, .1 });
    lisk_model_diffuse_color("shroom",  &(float[4]) { 1, 1, 1, .5 });
    lisk_model_specular_color("shroom",  &(float[4]) { 1, 1, 1, 1 }, 16.);
    lisk_model_show("shroom");

    lisk_directional_light_add(&(float[3]) { 0, -1, 0 },
            &(float[4]) { 1, 1, 1, 1 });
    lisk_handle_t p = lisk_point_light_add(&(float[3]) { 0, 0, 1 },
            &(float[4]) { 1, 0, 0, 1 });
    lisk_instance_light_point_set_attenuation(p, 1, .45, 0);

    lisk_instance_remove(shroom1);

    lisk_show();

    lisk_deinit();
}
