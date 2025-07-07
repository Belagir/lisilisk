
#include "3dful_core.h"
#include "geometry_parsing/3ful_geometry_parsing.h"

/** Buffer size used to read a file. No file might exceed 4kB ? */
#define GEOMETRY_FILEREAD_MAX_LENGTH (4096)

/**
 * @brief Allocates memory for a geometry object so it can store vertices and faces.
 *
 * @param[out] geometry
 */
void geometry_create(struct geometry *geometry)
{
    // TODO : set starting sizes to more sensible values
    *geometry = (struct geometry) {
        .vertices = range_create_dynamic(make_system_allocator(), sizeof(*geometry->vertices->data), 256),
        .faces    = range_create_dynamic(make_system_allocator(), sizeof(*geometry->faces->data), 256),
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
    BUFFER *buffer = range_create_dynamic(make_system_allocator(), sizeof(*buffer->data), GEOMETRY_FILEREAD_MAX_LENGTH);

    if (file_read(path, buffer) == 0) {
        geometry_wavobj_mem(geometry, buffer);
    }

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(buffer));
}

/**
 * @brief Loads geometry from a wavefront .obj file that has been loaded in memory.
 *
 * @param[inout] geometry
 * @param[in] obj
 */
void geometry_wavobj_mem(struct geometry *geometry, BUFFER *obj_buffer)
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
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->faces));

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
    glGenBuffers(1, &geometry->gpu_side.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->gpu_side.vbo);
    glBufferData(GL_ARRAY_BUFFER,
            geometry->vertices->length * sizeof(*geometry->vertices->data),
            geometry->vertices->data, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->gpu_side.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->gpu_side.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            geometry->faces->length * sizeof(*geometry->faces->data),
            geometry->faces->data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * @brief Quiery OpenGL to delete the buffers created during geometry_load().
 *
 * @param[inout] geometry
 */
void geometry_unload(struct geometry *geometry)
{
    glDeleteBuffers(1, &geometry->gpu_side.ebo);
    glDeleteBuffers(1, &geometry->gpu_side.vbo);

    geometry->gpu_side.ebo = 0;
    geometry->gpu_side.vbo = 0;
}

/**
 * @brief Adds an empty vertex to the geometry, filling an index used to reference it.
 *
 * @param[inout] geometry
 * @param[out] out_idx
 */
void geometry_push_vertex(struct geometry *geometry, u32 *out_idx)
{
    // TODO : ensure capacity
    range_push(RANGE_TO_ANY(geometry->vertices), &(struct vertex) { 0 });
    if (out_idx) *out_idx = (u32) geometry->vertices->length - 1;
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
    geometry->vertices->data[idx].pos = pos;
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
    geometry->vertices->data[idx].normal = normal;
}

/**
 * @brief Adds an empty face to the geometry, filling an index used to reference it.
 *
 * @param[inout] geometry
 * @param[out] out_idx
 */
void geometry_push_face(struct geometry *geometry, u32 *out_idx)
{
    // TODO : ensure capacity
    range_push(RANGE_TO_ANY(geometry->faces), &(struct face) { 0 });
    if (out_idx) *out_idx = (u32) geometry->faces->length - 1;
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
    geometry->faces->data[idx].idx_vert[0] = indices[0];
    geometry->faces->data[idx].idx_vert[1] = indices[1];
    geometry->faces->data[idx].idx_vert[2] = indices[2];
}
