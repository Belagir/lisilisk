
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(void)
{
    struct shader shader = { };
    struct geometry geometry = { };
    struct material material = { };
    struct object object = { };

    struct light_directional light_dir = { };
    struct light_point light_point = { };
    struct camera camera = { };
    struct scene scene = { };

    struct application target = application_create("some name", 800, 800);

    shader_frag(&shader, "shaders/geometry_material.frag");
    shader_vert(&shader, "shaders/geometry.vert");
    shader_link(&shader);

    geometry_create(&geometry);
    geometry_wavobj(&geometry, "models/monke.obj");
    geometry_load(&geometry);

    material_ambient(&material,  (f32[4]) { 1.0, 1.0, 1.0, 1.0 });
    material_diffuse(&material,  (f32[4]) { 1.0, 1.0, 1.0, 1.0 });
    material_specular(&material, (f32[4]) { 1.0, 1.0, 1.0, 1.0 });
    material_shininess(&material, 32.);
    material_load(&material);

    object_transform(&object, matrix4_identity());
    object_geometry(&object, &geometry);
    object_shader(&object, &shader);
    object_material(&object, &material);

    camera_projection(&camera, matrix4_get_projection_matrix(.1, 100, 45, 1));
    camera_view(&camera, matrix4_get_view_matrix((vector3) { 3, 2, 3 }, VECTOR3_ORIGIN, VECTOR3_Y_POSITIVE));

    light_color((struct light *)  &light_dir, (f32[4]) { 1., 1., 0., 1. });
    light_directional_direction(&light_dir, (vector3) { 0, -1, 0 });

    light_color((struct light *)  &light_point, (f32[4]) { .5, 1., 0., 1. }); // lime
    light_position(&light_point, (vector3) { 1, .5, 0 }); // orange

    scene_create(&scene);
    scene_camera(&scene, camera);
    scene_light_direc(&scene, light_dir);
    scene_light_point(&scene, light_point);
    scene_object(&scene, object);

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

    object_unload(&object);

    material_unload(&material);
    scene_unload(&scene);
    geometry_unload(&geometry);

    shader_delete(&shader);
    geometry_delete(&geometry);
    scene_delete(&scene);

    application_destroy(&target);

    return 0;
}
