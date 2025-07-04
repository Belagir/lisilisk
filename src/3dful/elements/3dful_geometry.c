
#include "3dful_core.h"

/**
 * @brief
 *
 * @param geometry
 */
void geometry_create(struct geometry *geometry)
{
    *geometry = (struct geometry) {
        .name     = range_create_dynamic(make_system_allocator(), sizeof(*geometry->name->data), 64),
        .vertices = range_create_dynamic(make_system_allocator(), sizeof(*geometry->vertices->data), 512),
        .faces    = range_create_dynamic(make_system_allocator(), sizeof(*geometry->faces->data), 256),
    };
}

/**
 * @brief
 *
 * @param geometry
 */
void geometry_delete(struct geometry *geometry)
{
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->vertices));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->normals));
    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(geometry->faces));

    *geometry = (struct geometry) { 0 };
}
