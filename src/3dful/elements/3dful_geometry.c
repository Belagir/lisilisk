
#include "3dful_core.h"
#include "geometry_parsing/3ful_geometry_parsing.h"

#define GEOMETRY_FILEREAD_MAX_LENGTH (4096)

/**
 * @brief
 *
 * @param geometry
 */
void geometry_create(struct geometry *geometry)
{
    *geometry = (struct geometry) {
        .vertices = range_create_dynamic(make_system_allocator(), sizeof(*geometry->vertices->data), 256),
        .faces    = range_create_dynamic(make_system_allocator(), sizeof(*geometry->faces->data), 256),
    };
}

/**
 * @brief
 *
 * @param geometry
 * @param path
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
 * @brief
 *
 * @param geometry
 * @param obj
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
 * @brief
 *
 * @param geometry
 */
void geometry_delete(struct geometry *geometry)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->faces));

    *geometry = (struct geometry) { 0 };
}

/**
 * @brief
 *
 * @param geometry
 * @param out_idx
 */
void geometry_push_vertex(struct geometry *geometry, u32 *out_idx)
{
    range_push(RANGE_TO_ANY(geometry->vertices), &(struct vertex) { 0 });
    if (out_idx) *out_idx = (u32) geometry->vertices->length - 1;
}

/**
 * @brief
 *
 * @param geometry
 * @param idx
 * @param pos
 */
void geometry_vertex_pos(struct geometry *geometry, size_t idx, vector3 pos)
{
    geometry->vertices->data[idx].pos = pos;
}

/**
 * @brief
 *
 * @param geometry
 * @param idx
 * @param normal
 */
void geometry_vertex_normal(struct geometry *geometry, size_t idx, vector3 normal)
{
    geometry->vertices->data[idx].normal = normal;
}

/**
 * @brief
 *
 * @param geometry
 * @param out_idx
 */
void geometry_push_face(struct geometry *geometry, u32 *out_idx)
{
    range_push(RANGE_TO_ANY(geometry->faces), &(struct face) { 0 });
    if (out_idx) *out_idx = (u32) geometry->faces->length - 1;
}

/**
 * @brief
 *
 * @param geometry
 * @param idx
 * @param indices
 */
void geometry_face_indices(struct geometry *geometry, size_t idx, u32 indices[3u])
{
    geometry->faces->data[idx].idx_vert[0] = indices[0];
    geometry->faces->data[idx].idx_vert[1] = indices[1];
    geometry->faces->data[idx].idx_vert[2] = indices[2];
}
