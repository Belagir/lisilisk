
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"
#include "3dful/collections/3dful_collections.h"

int main(void)
{
    struct shader shader = { };
    struct geometry geometry = { };
    struct material material = { };
    struct object object = { };

    struct instances instances = { };
    struct camera camera = { };

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
    material_send_uniforms(&material, &shader);

    object_transform(&object, matrix_translate(matrix4_identity(), (vector3) { -4, 0, 0 }));
    object_geometry(&object, &geometry);
    object_shader(&object, &shader);
    object_material(&object, &material);
    object_load(&object);

    instances_create(&instances);
    instances_of(&instances, &object);
    instances_push(&instances, matrix_translate(matrix4_identity(), (vector3) { 0.0, 0, 0 }));
    instances_push(&instances, matrix_translate(matrix4_identity(), (vector3) { 2.0, 0, 0 }));
    instances_push(&instances, matrix_translate(matrix4_identity(), (vector3) { 4.0, 0, 0 }));
    instances_push(&instances, matrix_translate(matrix4_identity(), (vector3) { 6.0, 0, 0 }));
    instances_load(&instances);

    camera_projection(&camera, matrix4_get_projection_matrix(.1, 100, 45, 1));
    camera_view(&camera, matrix4_get_view_matrix((vector3) { 6, 2, 6 }, VECTOR3_ORIGIN, VECTOR3_Y_POSITIVE));
    camera_send_uniforms(&camera, &shader);


    int should_quit = 0;
    SDL_Event event = { };
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            should_quit = event.type == SDL_QUIT;
        }

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            instances_draw(&instances);
            // object_draw(object);
        }
        SDL_GL_SwapWindow(target.sdl_window);

        SDL_Delay(100);
    }

    object_unload(&object);
    instances_unload(&instances);
    material_unload(&material);
    geometry_unload(&geometry);

    instances_delete(&instances);
    shader_delete(&shader);
    geometry_delete(&geometry);

    application_destroy(&target);

    return 0;
}
