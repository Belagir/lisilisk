
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

    lisk_model_geometry("shroom", "models/mushroom/mushroom.obj");
    lisk_model_ambient_color("shroom", &(float [4]) { 1, 1, 1, .1 });
    lisk_model_diffuse_color("shroom", &(float [4]) { 1, 1, 1, .8 });
    lisk_model_specular_color("shroom", &(float [4]) { 1, 1, 1, .8 }, 64);
    lisk_model_base_texture("shroom", "models/mushroom/ShroomBase.png");

    lisk_model_show("shroom");

    lisk_handle_t shroom = lisk_model_instanciate("shroom",
            &(float[3]) {  0, 0, 0 });
    lisk_instance_set_scale(shroom, .2);

    lisk_directional_light_add(&(float[3]) { -1, 0, 0 },
            &(float[4]) { 1, 1, 1, 1 });
    lisk_point_light_add(&(float[3]) { 0, 0, 1 },
            &(float[4]) { 1, 0, 0, 1 }, 1, .45, 0);

    lisk_show();

    lisk_deinit();
}
