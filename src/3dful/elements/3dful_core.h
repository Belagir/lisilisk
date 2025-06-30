
#ifndef SCRYING_ORB_CORE_H__
#define SCRYING_ORB_CORE_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/allocation.h>
#include <ustd/common.h>

#include "../inout/file_operations.h"

struct shader {
    GLuint name;
};

union vertex { struct { f32 x, y, z, w; }; f32 array[4u]; };
union color  { struct { f32 r, g, b; }; f32 array[3u]; };

/**
 * 
 */
struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(union color) *colors;
};

struct shader shader_compile_vertex(BUFFER *shader_source);
struct shader shader_compile_fragment(BUFFER *shader_source);

void geometry_from_wavefront_obj(BUFFER *buffer, struct geometry *out_geometry);



#endif
