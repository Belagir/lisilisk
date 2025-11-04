/**
 * @file lisilisk_internals.h
 * @author Gabriel Bédat
 * @brief
 * @version 0.1
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef LISILISK_INTERNALS_H__
#define LISILISK_INTERNALS_H__

#include <lisilisk.h>
#include <ustd/hashmap.h>

#include <ustd/filereading.h>
#include <ustd/math2d.h>
#include <ustd/math3d.h>
#include <ustd/path.h>

#include "../3dful/3dful.h"
#include "../resourceful/resourceful.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Kinds of objects managed by handles.
 *
 */
enum handle_flavor : u8 {
    HANDLE_IS_INVALID = 0,
    HANDLE_REPRESENTS_INSTANCE,
    HANDLE_REPRESENTS_LIGHT_DIREC,
    HANDLE_REPRESENTS_LIGHT_POINT,
    HANDLE_REPRESENTS_CAMERA,
};

enum res_flavor : u8 {
    RES_IS_INVALID = 0,
    RES_REPRESENTS_MODEL,
    RES_REPRESENTS_TEXTURE,
    RES_REPRESENTS_SHADER,
    RES_REPRESENTS_GEOMETRY,
    RES_REPRESENTS_MATERIAL,
};

/**
 * @brief Translation of a user-facing handle to the underlying usable data.
 *
 */
union lisk_handle_layout {
    lisk_entity_t full;
    struct {
        /** Hash associated to an object stored in the engine. */
        u32 hash:32;
        /** Eventual instance handle from the 3dful module. */
        u32 internal:HANDLE_BREADTH;
        /** Value from the enum handle_flavor. */
        enum handle_flavor flavor:8;
    };
};

union lisk_res_layout {
    lisk_res_t full;
    struct {
        /** Hash associated to an object stored in the engine. */
        u32 hash:32;
        /** Value from the enum handle_flavor. */
        enum res_flavor flavor:8;
    };
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Data store to cache texture objects.
 *
 */
struct lisilisk_store_texture {
    struct texture *default_texture;
    HASHMAP(struct texture *) textures;
};

/**
 * @brief Data store to cache geometry objects.
 *
 */
struct lisilisk_store_geometry {
    struct geometry *sphere;

    HASHMAP(struct geometry *) geometries;
};

/**
 * @brief Data store to cache material objects.
 *
 */
struct lisilisk_store_material {
    struct lisilisk_store_texture *texture_store;

    struct material *default_material;
    HASHMAP(struct material *) materials;
};

/**
 * @brief Data store to cache model objects.
 *
 */
struct lisilisk_store_model {
    struct lisilisk_store_material *material_store;
    struct lisilisk_store_shader *shader_store;

    HASHMAP(struct model *) models;
};

/**
 * @brief Data store to cache shader objects.
 *
 */
struct lisilisk_store_shader {
    struct shader *default_shader;

    HASHMAP(struct shader *) shaders;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Holds data about the OS-facing stuff needed by the engine.
 *
 */
struct lisilisk_context {
    struct SDL_Window *window;
    SDL_GLContext *opengl;
    struct resource_manager *res_manager;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


/** Face definition. Holds indices for vertices, normals and texture UVs. */
struct lisilisk_parse_obj_face { u32 v_idx[3], vn_idx[3], vt_idx[3]; };

/**
 * @brief Mirrors the content of a .obj file. This is a parser object that
 * serves as an intermediate between a bufferized file and an usable geometry.
 */
struct lisilisk_parse_obj {
    /** Vertices. */
    ARRAY(struct vector3) v_array;
    /** Normals. */
    ARRAY(struct vector3) vn_array;
    /** Texture UVs. */
    ARRAY(struct vector2) vt_array;
    /** Faces. */
    ARRAY(struct lisilisk_parse_obj_face) f_array;

    /** */
    ARRAY(char) mtllib;
    /** */
    ARRAY(char) usemtl;

    /** True if the model should be rendered smooth. */
    bool smooth;
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void lisilisk_setup_environment(
        struct environment *env,
        struct geometry *sky_shape,
        struct shader *sky_shader);
void lisilisk_setup_camera(
        struct camera *camera,
        struct lisilisk_context *context);

// -----------------------------------------------------------------------------

void lisilisk_context_init(
        struct lisilisk_context *context,
        struct logger *log,
        const char *name,                   // TODO : switch to ARRAY(const char)
        u32 width, u32 height);

void lisilisk_context_deinit(
        struct lisilisk_context *context);

void lisilisk_context_window_set_size(
        struct lisilisk_context *context,
        u32 width, u32 height);

void lisilisk_context_window_get_size(
        struct lisilisk_context *context,
        i32 *width, i32 *height);

void lisilisk_context_window_set_name(
        struct lisilisk_context *context,
        const char *name);                  // TODO : switch to ARRAY(const char)

void lisilisk_context_integrate_resources(
        struct lisilisk_context *context,
        const char *folder);                // TODO : switch to ARRAY(const char)

// -----------------------------------------------------------------------------

struct lisilisk_store_texture lisilisk_store_texture_create(void);
void lisilisk_store_texture_delete(
        struct lisilisk_store_texture *store);

struct texture *lisilisk_store_texture_cubemap_cache(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *(*images)[6]);

bool lisilisk_store_texture_register(
        struct lisilisk_store_texture *store,
        struct resource_manager *res_manager,
        const char *image,                  // TODO : switch to ARRAY(const char)
        u32 *out_hash);
struct texture *lisilisk_store_texture_retrieve(
        struct lisilisk_store_texture *store,
        u32 hash);

// -----------------------------------------------------------------------------

struct lisilisk_store_geometry lisilisk_store_geometry_create(void);
void lisilisk_store_geometry_delete(
        struct lisilisk_store_geometry *store);

bool lisilisk_store_geometry_register(
        struct lisilisk_store_geometry *store,
        struct resource_manager *res_manager,
        PATH obj_path,
        u32 *out_hash);

struct geometry *lisilisk_store_geometry_retrieve(
        struct lisilisk_store_geometry *store,
        u32 hash);

// -----------------------------------------------------------------------------

struct lisilisk_store_material lisilisk_store_material_create(
        struct lisilisk_store_texture *texture_store);
void lisilisk_store_material_delete(
        struct lisilisk_store_material *store);

bool lisilisk_store_material_register(
        struct lisilisk_store_material *store,
        const char *library,                // TODO : switch to ARRAY(const char)
        const char *name,                   // TODO : switch to ARRAY(const char)
        u32 *out_hash);
struct material *lisilisk_store_material_retrieve(
        struct lisilisk_store_material *store,
        u32 hash);

// -----------------------------------------------------------------------------

struct lisilisk_store_model lisilisk_store_model_create(
        struct lisilisk_store_material *material_store,
        struct lisilisk_store_shader *shader_store);
void lisilisk_store_model_delete(
        struct lisilisk_store_model *store);

bool lisilisk_store_model_register(
        struct lisilisk_store_model *store,
        const char *name,               // TODO : switch to ARRAY(const char)
        u32 *out_hash);
struct model *lisilisk_store_model_retrieve(
        struct lisilisk_store_model *store,
        u32 hash);

// -----------------------------------------------------------------------------

struct lisilisk_store_shader lisilisk_store_shader_create(void);
void lisilisk_store_shader_delete(
        struct lisilisk_store_shader *shader_store);

bool lisilisk_store_shader_register(
        struct lisilisk_store_shader *store,
        struct resource_manager *res_manager,
        const char *frag, const char *vert,         // TODO : switch to ARRAY(const char)
        u32 *out_hash);

struct shader *lisilisk_store_shader_retrieve(
        struct lisilisk_store_shader *store,
        u32 hash);

// -----------------------------------------------------------------------------

// Allocates memory for a parsing object.
void lisilisk_parse_obj_create(struct lisilisk_parse_obj *obj);
// Releases memory from a parsing object.
void lisilisk_parse_obj_delete(struct lisilisk_parse_obj *obj);
// Loads an obj file (already in a buffer) to a parsing object.
void lisilisk_parse_obj_parse(struct lisilisk_parse_obj *obj,
        const ARRAY(byte) buffer_array);
// Builds a geometry from parsed data.
void lisilisk_parse_obj_to(const struct lisilisk_parse_obj *obj,
        PATH local_path,
        struct geometry *geometry);
// Writes data parsed to a stream, in a form compatible with the .obj format.
void lisilisk_parse_obj_dump(const struct lisilisk_parse_obj *obj, FILE *file);

#endif
