
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"


void drawscene(struct application target)
{
    struct geometry diamond = { 0 };
    BUFFER *buffer_obj = NULL;
    BUFFER *buffer_vert = NULL;
    BUFFER *buffer_frag = NULL;
    struct shader_program shaders = { 0 }; 
    struct object diamond_object = { 0 };
    matrix4_t projection = { 0 };
    matrix4_t model = { 0 };

    buffer_obj = range_create_dynamic(make_system_allocator(), sizeof(*buffer_obj->data), 2048);
    buffer_vert = range_create_dynamic(make_system_allocator(), sizeof(*buffer_vert->data), 2048);
    buffer_frag = range_create_dynamic(make_system_allocator(), sizeof(*buffer_frag->data), 2048);

    diamond = geometry_create_empty(make_system_allocator());
    file_read("models/cube_triangles.obj", buffer_obj);
    geometry_from_wavefront_obj(buffer_obj, &diamond);

    file_read("shaders/dummy.vert", buffer_vert);
    file_read("shaders/dummy.frag", buffer_frag);
    shaders = shader_program_create(buffer_frag, buffer_vert, &target);

    diamond_object = object_create(diamond, shaders, &target);

    projection = matrix4_get_projection_matrix(0.1, 100, 90, 1.);
    model = matrix4_get_model_matrix(0, 0, -15, 1.);

    glUseProgram(diamond_object.shading.program);
    glUniformMatrix4fv(glGetUniformLocation(diamond_object.shading.program, "PROJECTION_MATRIX"), 1, GL_FALSE, (const GLfloat *) &projection);
    glUniformMatrix4fv(glGetUniformLocation(diamond_object.shading.program, "MODEL_MATRIX"), 1, GL_FALSE, (const GLfloat *) &model);
    glUseProgram(0);

    // -------------------------------------------------------------------------

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    object_render(diamond_object);
    SDL_GL_SwapWindow(target.sdl_window);
    SDL_Delay(2000);

    // -------------------------------------------------------------------------

    geometry_destroy(make_system_allocator(), &diamond);
    object_destroy(&diamond_object);

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_obj));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_vert));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer_frag));
}

int main(void)
{
    struct application target = application_create("some name", 800, 800);

    drawscene(target);

    application_destroy(&target);

    return 0;
}
