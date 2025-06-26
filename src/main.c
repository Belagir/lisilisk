
#include "3d/opengl_scenes.h"

int main(void)
{
    struct ogl_target target = ogl_target_create("some name", 1200,800);

    BUFFER *buffer = range_create_dynamic(make_system_allocator(),
            sizeof(*buffer->data), 2048);

    file_read("shaders/dummy.vert", buffer);
    struct shader vert = shader_compile_vertex(buffer);

    file_read("shaders/dummy.frag", buffer);
    struct shader frag = shader_compile_fragment(buffer);

    file_read("models/cube.obj", buffer);
    struct geometry geometry = create_geometry_empty(make_system_allocator());
    wavefront_obj_load_geometry(buffer, &geometry);

    struct object object = create_object_from_geometry(geometry);
    object_set_shaders(&object, vert, frag);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    render_object(object);

    SDL_GL_SwapWindow(target.sdl_window);
    SDL_Delay(3000);

    destroy_geometry(make_system_allocator(), &geometry);
    destroy_object(&object);
    shader_destroy(vert);
    shader_destroy(frag);

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
    ogl_target_destroy(&target);

    return 0;
}
