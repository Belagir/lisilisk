
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

enum shader_ubo_binding {
    SHADER_UBO_LIGHT_DIREC,
    SHADER_UBO_LIGHT_POINT,
    SHADER_UBO_MATERIAL,
};

enum shader_vertex_binding {
    SHADER_VERT_POS,
    SHADER_VERT_NORMAL,
    SHADER_VERT_INSTANCE,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores names of vertex, fragment, and whole shader program.
 *
 */
struct shader { GLuint frag_shader, vert_shader, program; };

// -------------------------------------------------------------------------------------------------

/**
 * @brief Piece of data passed to OpengGL to represent a vertice composing a mesh.
 *
 */
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

    struct {
        GLuint vbo;
        GLuint ebo;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Describes how some surface behaves in contact with light.
 * Passed to a material shader.
 */
struct material {
    struct {
        f32 ambient[4];
        f32 diffuse[4];
        f32 specular[4];
        f32 shininess;

        f32 PADDING[3];
    } properties;

    struct {
        GLuint ubo;
    } gpu_side;
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
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

struct instances {
    struct object *target;
    RANGE(matrix4) *transforms;

    struct {
        GLuint vbo;
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

/**
 * @brief Stores some light information.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned data for its Uniform Blocks Objects.
 *
 */
struct light {
    f32 color[4];
};

/**
 * @brief Stores data about a specific light that is emmited from a point in space.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned data for its Uniform Blocks Objects.
 *
 */
struct light_point {
    f32 color[4];
    vector3 position;

    f32 constant, linear, quadratic;

    f32 PADDING[1];
};

/**
 * @brief Stores data about a specific light that is emitted from very far away.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned data for its Uniform Blocks Objects.
 *
 */
struct light_directional {
    f32 color[4];
    vector3 direction;

    f32 PADDING[1];
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// TRANSFORMS --------------------------------------------------------------------------------------

void transform_translate(struct matrix4 *matrix, vector3 offset);
void position_translate(struct vector3 *pos, vector3 offset);

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
void geometry_delete(struct geometry *geometry);

void geometry_load(struct geometry *geometry);
void geometry_unload(struct geometry *geometry);

void geometry_wavobj(struct geometry *geometry, const char *path);
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj);

void geometry_push_vertex(struct geometry *geometry, u32 *out_idx);
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos);
void geometry_vertex_normal(struct geometry *geometry, size_t idx, vector3 normal);

void geometry_push_face(struct geometry *geometry, u32 *out_idx);
void geometry_face_indices(struct geometry *geometry, size_t idx, u32 indices[3u]);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// MATERIAL ----------------------------------------------------------------------------------------

void material_ambient(struct material *material, f32 ambient[4]);
void material_diffuse(struct material *material, f32 diffuse[4]);
void material_specular(struct material *material, f32 specular[4]);
void material_shininess(struct material *material, float shininess);

void material_load(struct material *material);
void material_unload(struct material *material);
void material_send_uniforms(struct material *material, struct shader *shader);

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
void object_send_uniforms(struct object *object, struct shader *shader);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// INSTANCES ---------------------------------------------------------------------------------------

void instances_create(struct instances *instances);
void instances_delete(struct instances *instances);
void instances_of(struct instances *instances, struct object *object);
void instances_load(struct instances *instances);
void instances_unload(struct instances *instances);
void instances_draw(struct instances *instances);
void instances_push(struct instances *instances, struct matrix4 transform);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CAMERA ------------------------------------------------------------------------------------------

void camera_projection(struct camera *camera, struct matrix4 projection);
void camera_view(struct camera *camera, struct matrix4 view);
void camera_send_uniforms(struct camera *camera, struct shader *shader);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// LIGHT -------------------------------------------------------------------------------------------

void light_color(struct light *light, f32 color[4]);

void light_position(struct light_point *light, struct vector3 pos);
void light_point_constant(struct light_point *light, f32 constant);
void light_point_linear(struct light_point *light, f32 linear);
void light_point_quadratic(struct light_point *light, f32 quadratic);

void light_directional_direction(struct light_directional *light, struct vector3 direction);

#endif
