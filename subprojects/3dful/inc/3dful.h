/**
 * @file 3dful.h
 * @author Gabriel Bédat
 * @brief Main header needed to orchestrate an OpenGL-powered scene.
 * @version 0.1
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef APPLICATION_3DFUL_H__
#define APPLICATION_3DFUL_H__

#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

#include <ustd/common.h>
#include <ustd/array.h>
#include <ustd/hashmap.h>
#include <ustd/logging.h>
#include <ustd/math2d.h>
#include <ustd/math3d.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define LOADABLE_FLAG_NONE   (0x0)  ///< Empty flag.
#define LOADABLE_FLAG_LOADED (0x1)  ///< Notifies that the object is loaded.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define HANDLE_BREADTH 24       ///< Bit length of a handle.
#define HANDLE_MAX 0xffffff     ///< Maximum value a handle can take.
typedef u32 handle_t;           ///< Public handle, stored on a type big enough.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 * Matches OpenGL's GL_TEXTURE_CUBE_MAP_[POSITIVE,NEGATIVE]_[X,Y,Z] order of
 * defines.
 */
enum cubemap_face {
    CUBEMAP_FACE_RIGHT,
    CUBEMAP_FACE_LEFT,
    CUBEMAP_FACE_TOP,
    CUBEMAP_FACE_BOTTOM,
    CUBEMAP_FACE_FRONT,
    CUBEMAP_FACE_BACK,

    CUBEMAP_FACES_NUMBER,
};

// -----------------------------------------------------------------------------

enum geometry_culling {
    GEOMETRY_CULL_NONE,
    GEOMETRY_CULL_FRONT,
    GEOMETRY_CULL_BACK,
};

// -----------------------------------------------------------------------------

enum geometry_layering {
    GEOMETRY_LAYER_NORMAL,
    GEOMETRY_LAYER_FRONT,
    GEOMETRY_LAYER_BACK,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief For data storing the load state of some object.
 * This structure only stores data about the load state of other data, and the
 * load procedure is entirely decoupled from functions taking this kind of data.
 * It is the responsability of the user to update this struct's data with
 * loadable_*() functions.
 */
struct loadable {
    /** @brief Load state flags. */
    u16 flags;
    /** @brief Number of times loadable_add_user() has been called. */
    u16 nb_users;
};

// -----------------------------------------------------------------------------

/**
 * @brief For contiguous data that can be loaded to an OpenGL buffer object.
 *
 */
struct handle_buffer_array {
    /** Data about the array load status on the GPU. */
    struct loadable load_state;

    u32 id_counter;

    /** Foreign-owned data. MUST be an array as defined in ustd/array.h. */
    ARRAY_ANY data_array;
    /** Owned handle array. MUST be an array as defined in ustd/array.h. */
    ARRAY(handle_t) handles;

    /** OpenGL name for the buffer object. Valid when the data is loaded. */
    GLuint buffer_name;
    /** User-specified buffer object usage. */
    GLenum buffer_usage;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Stores names of vertex, fragment, and whole shader program.
 *
 */
struct shader {
    GLuint frag_shader;
    GLuint vert_shader;
    GLuint program;
};

// -----------------------------------------------------------------------------

/**
 * @brief Piece of data passed to OpengGL to represent a vertice composing a
 * mesh.
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

    struct { u32 culling:2, smooth:1, layering:2, padding:27; } render_flags;

    ARRAY(struct vertex) vertices;
    ARRAY(struct face) faces;

    struct {
        // TODO: update data behind this vbo when the vertices_array changes (?)
        GLuint vbo;
        // TODO: update data behind this ebo when the face changes (?)
        GLuint ebo;
    } gpu_side;
};

// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
enum texture_flavor {
    TEXTURE_FLAVOR_2D,
    TEXTURE_FLAVOR_CUBEMAP,
};

/**
 * @brief
 *
 */
struct texture {
    struct loadable load_state;

    enum texture_flavor flavor;

    union {
        SDL_Surface *image_for_2D;
        SDL_Surface *images_for_cubemap[CUBEMAP_FACES_NUMBER];
    } specific;

    struct {
        GLuint name;
    } gpu_side;
};

// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct material_properties {
        f32 ambient[3], ambient_strength;
        f32 diffuse[3], diffuse_strength;
        f32 specular[3], specular_strength;
        f32 emissive[3], emissive_strength;
        f32 shininess;

        f32 PADDING[3];
};

struct material_user_uniform {
    const char *name;
    size_t nb;
    byte value[4*4*sizeof(f32)];
};

/**
 * @brief Describes how some surface behaves in contact with light.
 * Passed to a material shader.
 */
struct material {
    struct loadable load_state;

    struct material_properties properties;
    struct texture * samplers[16u];

    HASHMAP(struct material_user_uniform) added_uniforms;

    struct {
        GLuint ubo;
    } gpu_side;
};

// -----------------------------------------------------------------------------

struct instance {
    struct vector3 position;
    struct vector3 scale;
    struct quaternion rotation;
};

/**
 * @brief Stores data about a world model that can be rendered in the world.
 *
 */
struct model {
    struct loadable load_state;

    struct shader *shader;
    struct geometry *geometry;
    struct material *material;

    ARRAY(struct instance) instances_array;
    struct handle_buffer_array instances;

    // opengl names referencing the model's data on the gpu.
    struct {
        GLuint vao;
    } gpu_side;
};

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

/**
 * @brief Stores some light information.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned
 * data for its Uniform Blocks Objects.
 *
 */
struct light {
    f32 color[4];
};

/**
 * @brief Stores data about a specific light that is emmited from a point in
 * space.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned
 * data for its Uniform Blocks Objects.
 *
 */
struct light_point {
    f32 color[4];
    vector3 position;

    f32 PADDING_1[1];

    f32 constant, linear, quadratic;

    f32 PADDING_2[1];
};

/**
 * @brief Stores data about a specific light that is emitted from very far away.
 * @todo Keep those fields aligned to 16 bytes. OpenGL expects vec4-aligned
 * data for its Uniform Blocks Objects.
 *
 */
struct light_directional {
    f32 color[4];
    vector3 direction;

    f32 PADDING[1];
};

// -----------------------------------------------------------------------------

struct environment {
    struct loadable load_state;

    struct geometry *shape;
    struct shader *shader;

    struct texture *cube_texture;
    struct light ambient_light;

    f32 fog_color[3], fog_distance;
    f32 bg_color[3];

    struct {
        GLuint vao;
    } gpu_side;
};

// -----------------------------------------------------------------------------

/**
 * @brief Holds data about a scene. Models, lights, environment, and camera :
 * all that is needed to compose and render a scene of models to an opengl
 * context.
 *
 */
struct scene {
    struct loadable load_state;

    struct model * *models_array;

    struct camera *camera;

    struct {
        struct light_point *point_lights_array;
        struct handle_buffer_array point_lights;

        struct light_directional *direc_lights_array;
        struct handle_buffer_array direc_lights;
    } light_sources;

    struct environment *env;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void scene_create(struct scene *scene);
void scene_delete(struct scene *scene);

// -----------------------------------------------------------------------------

void scene_model(struct scene *scene, struct model *model);
void scene_camera(struct scene *scene, struct camera* camera);
void scene_environment(struct scene *scene, struct environment *env);

// -----------------------------------------------------------------------------

void scene_light_point(struct scene *scene, handle_t *out_handle);
void scene_light_point_position(struct scene *scene, handle_t handle,
        struct vector3 pos);
void scene_light_point_color(struct scene *scene, handle_t handle,
        f32 color[4]);
void scene_light_point_attenuation(struct scene *scene, handle_t handle,
        f32 constant, f32 linear, f32 quadratic);
void scene_light_point_remove(struct scene *scene, handle_t handle);

// -----------------------------------------------------------------------------

void scene_light_direc(struct scene *scene, handle_t *out_handle);
void scene_light_direc_orientation(struct scene *scene, handle_t handle,
        struct vector3 dir);
void scene_light_direc_color(struct scene *scene, handle_t handle,
        f32 color[4]);
void scene_light_direc_remove(struct scene *scene, handle_t handle);

// -----------------------------------------------------------------------------

void scene_draw(struct scene *scene, u32 time);

void scene_load(struct scene *scene);
void scene_unload(struct scene *scene);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SHADERS ---------------------------------------------------------------------

void shader_material_vert_mem(struct shader *shader, const byte *source,
        size_t length);
void shader_material_frag_mem(struct shader *shader, const byte *source,
        size_t length);

void shader_vert_mem(struct shader *shader, const byte *source,
        size_t length);
void shader_frag_mem(struct shader *shader, const byte *source,
        size_t length);

void shader_uniform_float(struct shader *shader, const char *name,
        float value);

void shader_link(struct shader *shader);
void shader_delete(struct shader *shader);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// GEOMETRY --------------------------------------------------------------------

void geometry_create(struct geometry *geometry);
void geometry_delete(struct geometry *geometry);

void geometry_wavobj(struct geometry *geometry, const char *path);
void geometry_wavobj_mem(struct geometry *geometry, const byte *obj_buffer,
        size_t length);

void geometry_set_smoothing(struct geometry *geometry, bool smooth);
void geometry_set_culling(struct geometry *geometry,
        enum geometry_culling cull);
void geometry_set_layering(struct geometry *geometry,
        enum geometry_layering layering);
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// TEXTURE ---------------------------------------------------------------------

void texture_2D_default(struct texture *texture);
void texture_2D_file(struct texture *texture, const char *path);
// TODO : array out !
void texture_2D_file_mem(struct texture *texture,
        const byte *image_buffer, size_t length);
void texture_cubemap_file(struct texture *texture, enum cubemap_face face,
        const char *path);
// TODO : array out !
void texture_cubemap_file_mem(struct texture *texture, enum cubemap_face face,
        const byte *image_buffer, size_t length);
void texture_delete(struct texture *texture);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// MATERIAL --------------------------------------------------------------------

void material_create(struct material *material);
void material_delete(struct material *material);

void material_texture(struct material *material, struct texture *texture);

void material_ambient(struct material *material, f32 ambient[3], f32 strength);
void material_ambient_mask(struct material *material, struct texture *mask);
void material_diffuse(struct material *material, f32 diffuse[3], f32 strength);
void material_diffuse_mask(struct material *material, struct texture *mask);
void material_specular(struct material *material, f32 specular[3],
        f32 strength);
void material_specular_mask(struct material *material, struct texture *mask);
void material_shininess(struct material *material, float shininess);
void material_emissive(struct material *material, f32 emission[3],
        f32 strength);
void material_emissive_mask(struct material *material, struct texture *mask);

void material_set_uniform_float(struct material *material, const char *name,
        size_t nb, f32 *values);

void material_custom_texture(struct material *material, u8 index,
        struct texture *texture);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// MODEL -----------------------------------------------------------------------

void model_create(struct model *model);
void model_delete(struct model *model);

void model_geometry(struct model *model, struct geometry *geometry);
void model_shader(struct model *model, struct shader *shader);
void model_material(struct model *model, struct material *material);

void model_instantiate(struct model *model, handle_t *out_handle);
void model_instance_remove(struct model *model, handle_t handle);
void model_instance_position(struct model *model, handle_t handle,
        struct vector3 pos);
void model_instance_rotation(struct model *model, handle_t handle,
        struct quaternion rotation);
void model_instance_scale(struct model *model, handle_t handle,
        f32 scale[3]);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CAMERA ----------------------------------------------------------------------

void camera_position(struct camera *camera, struct vector3 pos);
void camera_fov(struct camera *camera, f32 fov);
void camera_target(struct camera *camera, struct vector3 target);
void camera_limits(struct camera *camera, f32 near, f32 far);
void camera_aspect(struct camera *camera, f32 aspect);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// LIGHT -----------------------------------------------------------------------

void light_color(struct light *light, f32 color[4]);

void light_position(struct light_point *light, struct vector3 pos);
void light_point_constant(struct light_point *light, f32 constant);
void light_point_linear(struct light_point *light, f32 linear);
void light_point_quadratic(struct light_point *light, f32 quadratic);

void light_directional_direction(struct light_directional *light,
        struct vector3 direction);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// ENVIRONMENT -----------------------------------------------------------------

void environment_geometry(struct environment *env, struct geometry *shape);
void environment_ambient(struct environment *env, struct light light);
void environment_shader(struct environment *env, struct shader *shader);
void environment_skybox(struct environment *env, struct texture *cubemap);
void environment_fog(struct environment *env, f32 color[3], f32 distance);
void environment_bg(struct environment *env, f32 color[3]);

#endif
