
#include "3dful_core.h"

#include <ustd/array.h>

#include "geometry_parsing/3ful_geometry_parsing.h"

/**
 * @brief Allocates memory for a geometry object so it can store vertices and faces.
 *
 * @param[out] geometry
 */
void geometry_create(struct geometry *geometry)
{
    *geometry = (struct geometry) {
        .vertices_array = array_create(make_system_allocator(), sizeof(*geometry->vertices_array), 2),
        .faces_array    = array_create(make_system_allocator(), sizeof(*geometry->faces_array), 2),
    };
}

/**
 * @brief Loads geometry from a wavefront .obj file.
 * This operation will append the data from the file in the geometry object.
 *
 * @param[inout] geometry
 * @param[in] path
 */
void geometry_wavobj(struct geometry *geometry, const char *path)
{
    byte *buffer = array_create(make_system_allocator(), sizeof(*buffer), file_length(path));

    if (file_read_to_array(path, buffer) == 0) {
        geometry_wavobj_mem(geometry, buffer);
    }

    array_destroy(make_system_allocator(), (void **) &buffer);
}

/**
 * @brief Loads geometry from a wavefront .obj file that has been loaded in memory.
 *
 * @param[inout] geometry
 * @param[in] obj
 */
void geometry_wavobj_mem(struct geometry *geometry, const byte *obj_buffer)
{
    struct wavefront_obj obj = { };

    wavefront_obj_create(&obj);

    wavefront_obj_parse(&obj, obj_buffer);
    wavefront_obj_to(&obj, geometry);

    wavefront_obj_delete(&obj);
}

/**
 * @brief Releases all memory taken by some geometry, invalidating it.
 *
 * @param[inout] geometry
 */
void geometry_delete(struct geometry *geometry)
{
    array_destroy(make_system_allocator(), (void **) &geometry->vertices_array);
    array_destroy(make_system_allocator(), (void **) &geometry->faces_array);

    *geometry = (struct geometry) { 0 };
}

/**
 * @brief Sends geometry data to the GPU with OpenGL.
 * This will copy the geometry's vertices in gpu_side.vbo and the faces in gpu_side.ebo.
 *
 * @param[in] geometry
 */
void geometry_load(struct geometry *geometry)
{
    loadable_add_user((struct loadable *) geometry);

    if (!loadable_needs_loading((struct loadable *) geometry)) {
        return;
    }

    glGenBuffers(1, &geometry->gpu_side.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->gpu_side.vbo);
    glBufferData(GL_ARRAY_BUFFER,
            array_length(geometry->vertices_array) * sizeof(*geometry->vertices_array),
            geometry->vertices_array, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->gpu_side.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->gpu_side.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            array_length(geometry->faces_array) * sizeof(*geometry->faces_array),
            geometry->faces_array, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    geometry->load_state.flags |= LOADABLE_FLAG_LOADED;
}

/**
 * @brief Quiery OpenGL to delete the buffers created during geometry_load().
 *
 * @param[inout] geometry
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
 * @brief Adds an empty vertex to the geometry, filling an index used to reference it.
 *
 * @param[inout] geometry
 * @param[out] out_idx
 */
void geometry_push_vertex(struct geometry *geometry, u32 *out_idx)
{
    array_ensure_capacity(make_system_allocator(), (void **) &geometry->vertices_array, 1);
    array_push(geometry->vertices_array, &(struct vertex) { 0 });

    if (out_idx) *out_idx = (u32) array_length(geometry->vertices_array) - 1;
}

/**
 * @brief Sets a vertex's position to some value through its index.
 *
 * @param[inout] geometry
 * @param[in] idx
 * @param[in] pos
 */
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos)
{
    geometry->vertices_array[idx].pos = pos;
}

/**
 * @brief Sets a vertex's normal to some value through its index.
 *
 * @param[inout] geometry
 * @param[in] idx
 * @param[in] normal
 */
void geometry_vertex_normal(struct geometry *geometry, size_t idx, vector3 normal)
{
    geometry->vertices_array[idx].normal = normal;
}

/**
 * @brief
 *
 * @param geometry
 * @param idx
 * @param uv
 */
void geometry_vertex_uv(struct geometry *geometry, size_t idx, vector2 uv)
{
    geometry->vertices_array[idx].uv = uv;
}

/**
 * @brief
 *
 * @param geometry
 * @param smooth
 */
void geometry_set_smoothing(struct geometry *geometry, bool smooth)
{
    if (smooth) {
        geometry->render_flags |= GEOMETRY_RENDER_FLAG_SMOOTH;
    } else {
        geometry->render_flags &= ~GEOMETRY_RENDER_FLAG_SMOOTH;
    }
}

/**
 * @brief Adds an empty face to the geometry, filling an index used to reference it.
 *
 * @param[inout] geometry
 * @param[out] out_idx
 */
void geometry_push_face(struct geometry *geometry, u32 *out_idx)
{
    array_ensure_capacity(make_system_allocator(), (void **) &geometry->faces_array, 1);
    array_push(geometry->faces_array, &(struct face) { 0 });

    if (out_idx) *out_idx = (u32) array_length(geometry->faces_array) - 1;
}

/**
 * @brief Sets a face's indices to describe a triangle of vertices.
 * Use the indices filled by geometry_push_vertex() for the vertex indices.
 *
 * @param[inout] geometry
 * @param[in] idx
 * @param[in] indices
 */
void geometry_face_indices(struct geometry *geometry, size_t idx, u32 indices[3u])
{
    geometry->faces_array[idx].idx_vert[0] = indices[0];
    geometry->faces_array[idx].idx_vert[1] = indices[1];
    geometry->faces_array[idx].idx_vert[2] = indices[2];
}
