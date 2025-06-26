
#ifndef OBJECT_INTERNALS_H__
#define OBJECT_INTERNALS_H__

#include "../../inout/file_operations.h"

#include "object3d.h"

struct shader {
    GLuint shader_handle;
};

union vertex { struct { f32 x, y, z, w; }; f32 array[4u]; };
union color  { struct { f32 r, g, b; }; f32 array[3u]; };

struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(union color) *colors;
};

struct shader shader_compile_vertex(BUFFER *shader_source);
struct shader shader_compile_fragment(BUFFER *shader_source);

void shader_destroy(struct shader shader);

struct geometry create_geometry_empty(struct allocator alloc);
void destroy_geometry(struct allocator alloc, struct geometry *geometry);
void wavefront_obj_load_geometry(BUFFER *obj_source, struct geometry *out_geometry);

struct object create_object_from_geometry(struct geometry geometry);
void destroy_object(struct object *object);
void object_set_shaders(struct object *object, struct shader vertex, struct shader frag);
void render_object(struct object object);

#endif