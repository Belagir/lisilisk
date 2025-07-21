
#ifndef CORE_3DFUL_H__
#define CORE_3DFUL_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/math2d.h>
#include <ustd/math3d.h>

#include "../3dful.h"
#include "../inout/file_operations.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define LOADABLE_FLAG_NONE   (0x0)
#define LOADABLE_FLAG_LOADED (0x1)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Assigns integer values to semantic names for Uniform Buffer Objects binding indices.
 *
 */
enum shader_ubo_binding {
    SHADER_UBO_LIGHT_DIREC,
    SHADER_UBO_LIGHT_POINT,
    SHADER_UBO_MATERIAL,
};

/**
 * @brief  Assigns integer values to semantic names for vertex pointers.
 *
 */
enum shader_vertex_binding {
    SHADER_VERT_POS,
    SHADER_VERT_NORMAL,
    SHADER_VERT_UV,
    SHADER_VERT_INSTANCEMATRIX_ROW0,
    SHADER_VERT_INSTANCEMATRIX_ROW1,
    SHADER_VERT_INSTANCEMATRIX_ROW2,
    SHADER_VERT_INSTANCEMATRIX_ROW3,
};

/**
 * @brief This enumeration is a direct correspondance with the multiple
 * uniform sampler2D layout indices found in the `frag_head.glsl`.
 */
enum material_base_sampler {
    MATERIAL_BASE_SAMPLER_AMBIENT_MASK,
    MATERIAL_BASE_SAMPLER_SPECULAR_MASK,
    MATERIAL_BASE_SAMPLER_DIFFUSE_MASK,
    MATERIAL_BASE_SAMPLER_EMISSIVE_MASK,
    MATERIAL_BASE_SAMPLER_TEXTURE,

    MATERIAL_BASE_SAMPLERS_NUMBER,
};

/**
 * @brief
 * Matches OpenGL's GL_TEXTURE_CUBE_MAP_[POSITIVE,NEGATIVE]_[X,Y,Z] order of defines.
 */
enum skybox_face {
    SKYBOX_FACE_RIGHT,
    SKYBOX_FACE_LEFT,
    SKYBOX_FACE_TOP,
    SKYBOX_FACE_BOTTOM,
    SKYBOX_FACE_BACK,
    SKYBOX_FACE_FRONT,

    SKYBOX_FACES_NUMBER,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct loadable {
    u16 flags;
    u16 nb_users;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores names of vertex, fragment, and whole shader program.
 *
 */
struct shader {
    GLuint frag_shader;
    GLuint vert_shader;
    GLuint program;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Piece of data passed to OpengGL to represent a vertice composing a mesh.
 *
 */
struct vertex { struct vector3 pos, normal; struct vector2 uv; };

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
    struct loadable load_state;

    struct vertex *vertices_array;
    struct face *faces_array;

    struct {
        GLuint vbo;
        GLuint ebo;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct texture {
    struct loadable load_state;

    SDL_Surface *image;

    struct {
        GLuint name;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Describes how some surface behaves in contact with light.
 * Passed to a material shader.
 */
struct material {
    struct loadable load_state;

    struct {
        f32 ambient[3], ambient_strength;
        f32 diffuse[3], diffuse_strength;
        f32 specular[3], specular_strength;
        f32 emissive[3], emissive_strength;
        f32 shininess;

        f32 PADDING[3];
    } properties;

    struct texture * samplers[16u];

    struct {
        GLuint ubo;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores data about a world model that can be rendered in the world.
 *
 */
struct model {
    struct loadable load_state;

    struct shader *shader;
    struct geometry *geometry;
    struct material *material;

    struct matrix4 *tr_instances_array;

    // opengl names referencing the model's data on the gpu.
    struct {
        GLuint vao;
        GLuint vbo_instances;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores a camera view & projection matrices.
 *
 */
struct camera {
    vector3 pos;
    f32 fov, aspect;
    vector3 target;
    f32 near, far;

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

struct environment {
    struct shader *shader;
    struct camera *camera;

    struct texture *cubemap[SKYBOX_FACES_NUMBER];
    struct light ambient_light;

    struct {
        GLuint vao;
    } gpu_side;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// TRANSFORMS --------------------------------------------------------------------------------------

void transform_translate(struct matrix4 *matrix, vector3 offset);
void position_translate(struct vector3 *pos, vector3 offset);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// GENERAL LOADABLES -------------------------------------------------------------------------------

void loadable_add_user(struct loadable *obj);
void loadable_remove_user(struct loadable *obj);
i32 loadable_needs_loading(struct loadable *obj);
i32 loadable_needs_unloading(struct loadable *obj);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// SHADERS -----------------------------------------------------------------------------------------

void shader_material_vert_mem(struct shader *shader, byte *source);
void shader_material_vert(struct shader *shader, const char *path);
void shader_material_frag_mem(struct shader *shader, byte *source);
void shader_material_frag(struct shader *shader, const char *path);

void shader_vert_mem(struct shader *shader, byte *source);
void shader_vert(struct shader *shader, const char *path);
void shader_frag_mem(struct shader *shader, byte *source);
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
void geometry_wavobj_mem(struct geometry *geometry, byte *obj);

void geometry_push_vertex(struct geometry *geometry, u32 *out_idx);
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos);
void geometry_vertex_normal(struct geometry *geometry, size_t idx, vector3 normal);
void geometry_vertex_uv(struct geometry *geometry, size_t idx, vector2 uv);

void geometry_push_face(struct geometry *geometry, u32 *out_idx);
void geometry_face_indices(struct geometry *geometry, size_t idx, u32 indices[3u]);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// TEXTURE -----------------------------------------------------------------------------------------

void texture_default(struct texture *texture);
void texture_file(struct texture *texture, const char *path);
void texture_file_mem(struct texture *texture, const byte *image);
void texture_delete(struct texture *texture);

void texture_load(struct texture *texture);
void texture_unload(struct texture *texture);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// MATERIAL ----------------------------------------------------------------------------------------

void material_texture(struct material *material, struct texture *texture);

void material_ambient(struct material *material, f32 ambient[3], f32 strength);
void material_ambient_mask(struct material *material, struct texture *mask);
void material_diffuse(struct material *material, f32 diffuse[3], f32 strength);
void material_diffuse_mask(struct material *material, struct texture *mask);
void material_specular(struct material *material, f32 specular[3], f32 strength);
void material_specular_mask(struct material *material, struct texture *mask);
void material_shininess(struct material *material, float shininess);
void material_emissive(struct material *material, f32 emission[3], f32 strength);
void material_emissive_mask(struct material *material, struct texture *mask);

void material_custom_texture(struct material *material, u8 index, struct texture *texture);

void material_load(struct material *material);
void material_unload(struct material *material);
void material_bind_uniform_blocks(struct material *material, struct model *model);
void material_bind_textures(struct material *material, struct model *model);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// MODEL -------------------------------------------------------------------------------------------

void model_create(struct model *model);
void model_delete(struct model *model);

void model_geometry(struct model *model, struct geometry *geometry);
void model_shader(struct model *model, struct shader *shader);
void model_material(struct model *model, struct material *material);

void model_instantiate(struct model *model, struct matrix4 tr);

void model_load(struct model *model);
void model_unload(struct model *model);
void model_draw(struct model model);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CAMERA ------------------------------------------------------------------------------------------

void camera_position(struct camera *camera, struct vector3 pos);
void camera_fov(struct camera *camera, f32 fov);
void camera_target(struct camera *camera, struct vector3 target);
void camera_limits(struct camera *camera, f32 near, f32 far);
void camera_aspect(struct camera *camera, f32 aspect);
void camera_send_uniforms(struct camera *camera, struct model *model);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// LIGHT -------------------------------------------------------------------------------------------

void light_color(struct light *light, f32 color[4]);

void light_position(struct light_point *light, struct vector3 pos);
void light_point_constant(struct light_point *light, f32 constant);
void light_point_linear(struct light_point *light, f32 linear);
void light_point_quadratic(struct light_point *light, f32 quadratic);

void light_directional_direction(struct light_directional *light, struct vector3 direction);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ENVIRONMENT -------------------------------------------------------------------------------------

void environment_skybox(struct environment *env, struct light light);
void environment_camera(struct environment *env, struct camera *camera);
void environment_shader(struct environment *env, struct shader *shader);
void environment_ambient(struct environment *env, struct texture *(*cubemap)[6u]);

void environment_draw(struct environment *env);

void environment_load(struct environment *env);
void environment_unload(struct environment *env);

#endif
