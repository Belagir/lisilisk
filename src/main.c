
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"


void drawscene(struct application target)
{
    struct geometry diamond = geometry_create_empty(make_system_allocator());
    BUFFER *buffer_obj = range_create_dynamic(make_system_allocator(), sizeof(*buffer_obj->data), 512);
    file_read("models/diamond.obj", buffer_obj);
    geometry_from_wavefront_obj(buffer_obj, &diamond);

    BUFFER *buffer_vert = range_create_dynamic(make_system_allocator(), sizeof(*buffer_vert->data), 2048);
    BUFFER *buffer_frag = range_create_dynamic(make_system_allocator(), sizeof(*buffer_frag->data), 2048);

    file_read("shaders/dummy.vert", buffer_vert);
    file_read("shaders/dummy.frag", buffer_frag);
    struct shader_program shaders = shader_program_create(buffer_frag, buffer_vert);

    struct object diamond_object = object_create(diamond, shaders);


    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    object_render(diamond_object);
    SDL_GL_SwapWindow(target.sdl_window);
    SDL_Delay(2000);


    geometry_destroy(make_system_allocator(), &diamond);
    object_destroy(&diamond_object);

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_obj));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_vert));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_frag));
}

int main(void)
{
    struct application target = application_create("some name", 1200,800);

    drawscene(target);

    application_destroy(&target);

    return 0;
}
