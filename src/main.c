
#include "3dful/3dful.h"
#include "3dful/elements/3dful_core.h"


void drawscene(struct application target)
{
    int i; /* Simple iterator */
    GLuint vao, vbo[2]; /* Create handles for our Vertex Array Object and two Vertex Buffer Objects */

    /* We're going to create a simple diamond made from lines */
    const GLfloat diamond[4][4] = {
            {  0.0,  1.0, 0.0, 1.0  }, /* Top point */
            {  1.0,  0.0, 0.0, 1.0  }, /* Right point */
            {  0.0, -1.0, 0.0, 1.0  }, /* Bottom point */
            { -1.0,  0.0, 0.0, 1.0  } }; /* Left point */

    const GLfloat colors[4][3] = {
            {  1.0,  0.0,  0.0  }, /* Red */
            {  0.0,  1.0,  0.0  }, /* Green */
            {  0.0,  0.0,  1.0  }, /* Blue */
            {  1.0,  1.0,  1.0  } }; /* White */


    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);

    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);

    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(2, vbo);

    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    /* Copy the vertex data from diamond to our buffer */
    /* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);

    /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray(0);

    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    /* Copy the color data from colors to our buffer */
    /* 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values */
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    /* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /* Enable attribute index 1 as being used */
    glEnableVertexAttribArray(1);

    BUFFER *buffer_vert = range_create_dynamic(make_system_allocator(), sizeof(*buffer_vert->data), 2048);
    BUFFER *buffer_frag = range_create_dynamic(make_system_allocator(), sizeof(*buffer_frag->data), 2048);


    file_read("shaders/dummy.vert", buffer_vert);
    file_read("shaders/dummy.frag", buffer_frag);
    struct shader_program shaders = shader_program_create(buffer_frag, buffer_vert);


    /* Load the shader into the rendering pipeline */
    glUseProgram(shaders.program);

    /* Loop our display increasing the number of shown vertexes each time.
     * Start with 2 vertexes (a line) and increase to 3 (a triangle) and 4 (a diamond) */
    for (i=2; i <= 4; i++)
    {
        /* Make our background black */
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes */
        glDrawArrays(GL_LINE_LOOP, 0, i);

        /* Swap our buffers to make our changes visible */
        SDL_GL_SwapWindow(target.sdl_window);

        /* Sleep for 2 seconds */
        SDL_Delay(2000);
    }

    /* Cleanup all the things we bound and allocated */
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);

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
