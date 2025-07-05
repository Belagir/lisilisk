
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(void)
{
    struct shader shader = { };
    struct geometry geometry = { };
    struct object object = { };
    struct object object2 = { };
    struct camera camera = { };
    struct scene scene = { };
    struct material material = { };

    struct application target = application_create("some name", 800, 800);

    shader_frag(&shader, "shaders/dummy.frag");
    shader_vert(&shader, "shaders/dummy.vert");
    shader_link(&shader);

    geometry_create(&geometry);
    geometry_wavobj(&geometry, "models/cube_triangles.obj");

    material_ambient(&material, (vector3) { .1, .1, .1 });
    material_diffuse(&material, (vector3) { .2, .4, .0 });
    material_specular(&material, (vector3) { .3, .5, .1 });
    material_shininess(&material, 16.);

    object_transform(&object, matrix_translate(matrix4_identity(), (vector3) { .5, .5, 0 }));
    object_geometry(&object, &geometry);
    object_shader(&object, &shader);
    object_material(&object, &material);

    object_transform(&object2, matrix_translate(matrix4_identity(), (vector3) { -4, -.5, 0 }));
    object_geometry(&object2, &geometry);
    object_shader(&object2, &shader);
    object_material(&object2, &material);

    camera_projection(&camera, matrix4_get_projection_matrix(.1, 100, 45, 1));
    camera_view(&camera, matrix4_get_view_matrix((vector3) { 6, 5, 10 }, VECTOR3_Z_NEGATIVE, VECTOR3_Y_POSITIVE));

    scene_create(&scene);
    scene_camera(&scene, camera);
    scene_ambient_light_color(&scene, (f32[3]) { 1, 1, 1 });
    scene_point_light_pos(&scene, (vector3) { 0, 2, 0 });
    scene_add(&scene, object);
    scene_add(&scene, object2);
    scene_load(&scene);

    int should_quit = 0;
    SDL_Event event = { };
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            scene_draw(scene);
        }
        SDL_GL_SwapWindow(target.sdl_window);

        SDL_Delay(100);
    }

    shader_delete(&shader);
    geometry_delete(&geometry);
    scene_delete(&scene);

    application_destroy(&target);

    return 0;
}
