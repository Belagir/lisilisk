
#ifndef CORE_3DFUL_H__
#define CORE_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/math3d.h>
#include <ustd/logging.h>

#include "../3dful.h"
#include "../inout/file_operations.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores names of vertex, fragment, and whole shader program.
 *
 */
struct shader { GLuint frag_shader, vert_shader, program; };

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores vertex data.
 *
 */
union vertex { struct { f32 x, y, z; }; f32 array[3u]; };

/**
 * @brief Stores indices of a vertices array to describe triangular faces.
 *
 */
struct face  { u32 indices[3u]; };

/**
 * @brief Stores a single mesh's data.
 *
 */
struct geometry {
    RANGE(char) *name;
    RANGE(union vertex) *vertices;
    RANGE(struct face) *faces;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores data about a world object that can be rendered in the world.
 *
 */
struct object {
    struct matrix4_t transform;
    struct shader *shader;
    struct geometry *geometry;

    // opengl names referencing the object's data on the gpu.
    struct {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
    } gpu_side;

    // opengl uniform names.
    struct {
        GLuint model;
    } uniforms;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores a camera view & projection matrices.
 *
 */
struct camera {
    struct matrix4_t view;
    struct matrix4_t projection;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// SHADERS -----------------------------------------------------------------------------------------

void shader_vert_mem(struct shader *shader, BUFFER *source);
void shader_frag_mem(struct shader *shader, BUFFER *source);
void shader_vert(struct shader *shader, const char *path);
void shader_frag(struct shader *shader, const char *path);
void shader_link(struct shader *shader);
void shader_delete(struct shader *shader);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// GEOMETRY ----------------------------------------------------------------------------------------


void geometry_wavobj(struct geometry *geometry, const char *path);
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj);
void geometry_delete(struct geometry *geometry);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// OBJECT ------------------------------------------------------------------------------------------

void object_transform(struct object *object, struct matrix4_t transform);
void object_geometry(struct object *object, struct geometry *geometry);
void object_shader(struct object *object, struct shader *shader);
void object_load(struct object *object);
void object_unload(struct object *object);
void object_draw(struct object object);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CAMERA ------------------------------------------------------------------------------------------

void camera_projection(struct camera *camera, struct matrix4_t projection);
void camera_view(struct camera *camera, struct matrix4_t view);

#endif
