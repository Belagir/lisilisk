/**
 * @file 3dful_geometry.c
 * @author Gabriel Bédat
 * @brief Implementation of geometry-related procedures.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "3dful_core.h"

#include <ustd/array.h>

#include "geometry_parsing/3ful_geometry_parsing.h"

/**
 * @brief Allocates memory for a geometry object so it can store vertices and
 * faces.
 *
 * @param[out] geometry New geometry object.
 */
void geometry_create(struct geometry *geometry)
{
    *geometry = (struct geometry) {
        .render_flags = { .smooth = 0, .culling = GEOMETRY_CULL_BACK },
        .vertices = array_create(make_system_allocator(),
                sizeof(*geometry->vertices), 2),
        .faces    = array_create(make_system_allocator(),
                sizeof(*geometry->faces), 2),
    };
}

/**
 * @brief Loads geometry from a wavefront .obj file.
 * This operation will append the data from the file in the geometry object.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] path Path to some .obj file.
 */
void geometry_wavobj(struct geometry *geometry, const char *path)
{
    byte *buffer = array_create(make_system_allocator(), sizeof(*buffer),
            file_length(path));

    if (file_read_to_array(path, buffer) == 0) {
        geometry_wavobj_mem(geometry, buffer, array_length(buffer));
    }

    array_destroy(make_system_allocator(), (ARRAY_ANY *) &buffer);
}

/**
 * @brief Loads geometry from a wavefront .obj file that has been loaded in
 * memory.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] obj Array of bytes. Must have been created with ustd/array.h.
 */
void geometry_wavobj_mem(struct geometry *geometry, const byte *obj_buffer,
        size_t length)
{
    struct allocator alloc = make_system_allocator();
    struct wavefront_obj obj = { };
    ARRAY(byte) buffer = nullptr;

    wavefront_obj_create(&obj);
    buffer = array_create(alloc, sizeof(*buffer), length);

    array_append_mem(buffer, obj_buffer, length);
    wavefront_obj_parse(&obj, buffer);
    wavefront_obj_to(&obj, geometry);

    array_destroy(alloc, (ARRAY_ANY *) &buffer);
    wavefront_obj_delete(&obj);
}

/**
 * @brief Releases all memory taken by some geometry, invalidating it.
 *
 * @param[inout] geometry Released geometry.
 */
void geometry_delete(struct geometry *geometry)
{
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &geometry->vertices);
    array_destroy(make_system_allocator(), (ARRAY_ANY *) &geometry->faces);

    *geometry = (struct geometry) { 0 };
}

/**
 * @brief Sends geometry data to the GPU with OpenGL.
 * This will copy the geometry's vertices in gpu_side.vbo and the faces
 * in gpu_side::ebo.
 *
 * @param[in] geometry Loaded geometry.
 */
void geometry_load(struct geometry *geometry)
{
    loadable_add_user((struct loadable *) geometry);

    if (!loadable_needs_loading((struct loadable *) geometry)) {
        return;
    }

    glGenBuffers(1, &geometry->gpu_side.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->gpu_side.vbo);
    glBufferData(GL_ARRAY_BUFFER, array_length(geometry->vertices)
            * sizeof(*geometry->vertices), geometry->vertices,
            GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->gpu_side.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->gpu_side.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, array_length(geometry->faces)
            * sizeof(*geometry->faces), geometry->faces,
            GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    geometry->load_state.flags |= LOADABLE_FLAG_LOADED;
}

/**
 * @brief Quiery OpenGL to delete the buffers created during geometry_load().
 *
 * @param[inout] geometry Unloaded geometry.
 */
void geometry_unload(struct geometry *geometry)
{
    loadable_remove_user((struct loadable *) geometry);

    if (!loadable_needs_unloading((struct loadable *) geometry)) {
        return;
    }

    glDeleteBuffers(1, &geometry->gpu_side.ebo);
    glDeleteBuffers(1, &geometry->gpu_side.vbo);

    geometry->gpu_side.ebo = 0;
    geometry->gpu_side.vbo = 0;

    geometry->load_state.flags &= ~LOADABLE_FLAG_LOADED;
}

/**
 * @brief Adds an empty vertex to the geometry, filling an index used to
 * reference it.
 *
 * @param[inout] geometry Modified geometry.
 * @param[out] out_idx Outgoing index of the new vertex.
 */
void geometry_push_vertex(struct geometry *geometry, u32 *out_idx)
{
    array_ensure_capacity(make_system_allocator(),
            (void **) &geometry->vertices, 1);
    array_push(geometry->vertices, &(struct vertex) { 0 });

    if (out_idx) *out_idx = (u32) array_length(geometry->vertices) - 1;
}

/**
 * @brief Sets a vertex's position to some value through its index.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] idx Index of the modified vertex.
 * @param[in] pos Position of the vertex.
 */
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos)
{
    geometry->vertices[idx].pos = pos;
}

/**
 * @brief Sets a vertex's normal to some value through its index.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] idx Index of the modified vertex.
 * @param[in] normal Normal of the vertex.
 */
void geometry_vertex_normal(struct geometry *geometry, size_t idx,
        vector3 normal)
{
    geometry->vertices[idx].normal = normal;
}

/**
 * @brief Sets a vertex's texture UV to some value through its index.
 *
 * @param[inout] geometry Modified geometry.
 * @param idx Index of the modified vertex.
 * @param uv Uniform Vector of the vertex.
 */
void geometry_vertex_uv(struct geometry *geometry, size_t idx, vector2 uv)
{
    geometry->vertices[idx].uv = uv;
}

/**
 * @brief Sets the smoothing mode of this geometry.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] smooth true for render smooth, false for render flat.
 */
void geometry_set_smoothing(struct geometry *geometry, bool smooth)
{
    geometry->render_flags.smooth = smooth & 0x1;
}

/**
 * @brief
 *
 * @param geometry
 * @param cull
 */
void geometry_set_culling(struct geometry *geometry,
        enum geometry_culling cull)
{
    geometry->render_flags.culling = cull & 0x3;
}

/**
 * @brief
 *
 * @param geometry
 * @param layering
 */
void geometry_set_layering(struct geometry *geometry,
        enum geometry_layering layering)
{
    geometry->render_flags.layering = layering & 0x3;
}

/**
 * @brief Adds an empty face to the geometry, filling an index used to
 * reference it.
 *
 * @param[inout] geometry Modified geometry.
 * @param[out] out_idx outgoing face index.
 */
void geometry_push_face(struct geometry *geometry, u32 *out_idx)
{
    array_ensure_capacity(make_system_allocator(),
            (void **) &geometry->faces, 1);
    array_push(geometry->faces, &(struct face) { 0 });

    if (out_idx) *out_idx = (u32) array_length(geometry->faces) - 1;
}

/**
 * @brief Sets a face's indices to describe a triangle of vertices.
 * Use the indices filled by geometry_push_vertex() for the vertex indices.
 *
 * @param[inout] geometry Modified geometry.
 * @param[in] idx Modified face.
 * @param[in] indices Indices of the vertex composing the face.
 */
void geometry_face_indices(struct geometry *geometry, size_t idx,
        u32 indices[3u])
{
    geometry->faces[idx].idx_vert[0] = indices[0];
    geometry->faces[idx].idx_vert[1] = indices[1];
    geometry->faces[idx].idx_vert[2] = indices[2];
}
