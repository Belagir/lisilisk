
#include "3d/opengl_scenes.h"

int main(void)
{
    struct ogl_target target = ogl_target_create("some name", 1200,800);

    BUFFER *source_vert = range_create_dynamic(make_system_allocator(),
            sizeof(*source_vert->data), 2048);
    file_read("shaders/dummy.vert", source_vert);

    BUFFER *source_frag = range_create_dynamic(make_system_allocator(),
            sizeof(*source_frag->data), 2048);
    file_read("shaders/dummy.frag", source_frag);

    shader_destroy(shader_compile_vertex(source_vert));
    shader_destroy(shader_compile_fragment(source_frag));

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(source_vert));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(source_frag));
    ogl_target_destroy(&target);

    return 0;
}
