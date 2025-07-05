
#ifndef CORE_3DFUL_H__
#define CORE_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/math3d.h>

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

struct vertex {
    struct vector3 pos;
    struct vector3 normal;
};

/**
 * @brief Stores indices of a vertices array to describe triangular faces.
 *
 */
struct face { u32 idx_vert[3u]; };

/**
 * @brief Stores a single mesh's data.
 *
 */
struct geometry {
    RANGE(struct vertex) *vertices;
    RANGE(struct face) *faces;
};

// -------------------------------------------------------------------------------------------------

struct material {
    vector3 ambient;
    vector3 diffuse;
    vector3 specular;
    float shininess;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores data about a world object that can be rendered in the world.
 *
 */
struct object {
    struct matrix4 transform;

    struct shader *shader;
    struct geometry *geometry;
    struct material *material;

    // opengl names referencing the object's data on the gpu.
    struct {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores a camera view & projection matrices.
 *
 */
struct camera {
    struct matrix4 view;
    struct matrix4 projection;
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

void geometry_create(struct geometry *geometry);
void geometry_wavobj(struct geometry *geometry, const char *path);
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj);
void geometry_delete(struct geometry *geometry);

void geometry_push_vertex(struct geometry *geometry, u32 *out_idx);
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos);
void geometry_vertex_normal(struct geometry *geometry, size_t idx, vector3 normal);

void geometry_push_face(struct geometry *geometry, u32 *out_idx);
void geometry_face_indices(struct geometry *geometry, size_t idx, u32 indices[3u]);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// MATERIAL ----------------------------------------------------------------------------------------

void material_ambient(struct material *material, vector3 ambient);
void material_diffuse(struct material *material, vector3 diffuse);
void material_specular(struct material *material, vector3 specular);
void material_shininess(struct material *material, float shininess);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// OBJECT ------------------------------------------------------------------------------------------

void object_transform(struct object *object, struct matrix4 transform);
void object_geometry(struct object *object, struct geometry *geometry);
void object_shader(struct object *object, struct shader *shader);
void object_material(struct object *object, struct material *material);
void object_load(struct object *object);
void object_unload(struct object *object);
void object_draw(struct object object);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CAMERA ------------------------------------------------------------------------------------------

void camera_projection(struct camera *camera, struct matrix4 projection);
void camera_view(struct camera *camera, struct matrix4 view);

#endif
