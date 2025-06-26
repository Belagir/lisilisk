
#ifndef OBJECT3D_H__
#define OBJECT3D_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>

struct object {
    GLuint vao;

    GLuint vbo[2];

    GLuint shader_program;
    size_t vertices_nb;
};


#endif
