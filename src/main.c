
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(void)
{
    struct shader shader = { };
    struct geometry geometry = { };
    struct material material = { };

    struct object object = { };
    struct object object2 = { };
    struct camera camera = { };
    struct scene scene = { };

    struct light_directional light_dir = { };
    struct light_point light_point = { };

    struct application target = application_create("some name", 800, 800);

    shader_frag(&shader, "shaders/geometry_material.frag");
    shader_vert(&shader, "shaders/geometry.vert");
    shader_link(&shader);

    geometry_create(&geometry);
    geometry_wavobj(&geometry, "models/monke.obj");

    material_ambient(&material,  (vector3) { 1.0, 0.5, 0.3 });
    material_diffuse(&material,  (vector3) { 1.0, 0.5, 0.3 });
    material_specular(&material, (vector3) { 0.8, 0.1, 0.1 });
    material_shininess(&material, 64.);

    object_transform(&object, matrix_translate(matrix4_identity(), (vector3) { -1.5, 0, 0 }));
    object_geometry(&object, &geometry);
    object_shader(&object, &shader);
    object_material(&object, &material);

    object_transform(&object2, matrix_translate(matrix4_identity(), (vector3) { 1.5, 0, 0 }));
    object_geometry(&object2, &geometry);
    object_shader(&object2, &shader);
    object_material(&object2, &material);

    camera_projection(&camera, matrix4_get_projection_matrix(.1, 100, 45, 1));
    camera_view(&camera, matrix4_get_view_matrix((vector3) { 3, 2, 3 }, VECTOR3_ORIGIN, VECTOR3_Y_POSITIVE));

    light_color((struct light *)  &light_dir, (f32[4]) { 1., 1., 1., .3 });
    light_directional_direction(&light_dir, (vector3) { -.6, -1, -.8 });
    light_color((struct light *)  &light_point, (f32[4]) { 1., 1., 1., .3 });
    light_position((struct light *)  &light_point, (vector3) { 0, .5, 2 });

    scene_create(&scene);
    scene_camera(&scene, camera);
    scene_light_direc(&scene, light_dir);
    scene_light_point(&scene, light_point);
    scene_object(&scene, object);
    scene_object(&scene, object2);

    geometry_load(&geometry);
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

    scene_unload(&scene);
    geometry_unload(&geometry);

    shader_delete(&shader);
    geometry_delete(&geometry);
    scene_delete(&scene);

    application_destroy(&target);

    return 0;
}
