
#include "3d/opengl_scenes.h"

int main(void)
{
    struct ogl_target target = ogl_target_create("some name", 1200,800);

    BUFFER *buffer = range_create_dynamic(make_system_allocator(),
            sizeof(*buffer->data), 2048);

    file_read("shaders/dummy.vert", buffer);
    shader_destroy(shader_compile_vertex(buffer));


    file_read("shaders/dummy.frag", buffer);
    shader_destroy(shader_compile_fragment(buffer));

    struct geometry obj = create_geometry_empty(make_system_allocator());
    file_read("models/cube.obj", buffer);
    wavefront_obj_load_geometry(buffer, &obj);
    destroy_geometry(make_system_allocator(), &obj);

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
    ogl_target_destroy(&target);

    return 0;
}
