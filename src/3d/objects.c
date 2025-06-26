
#include "opengl_scenes.h"

/**
 * @brie
 *
 * @param alloc
 * @return struct object
 */
struct geometry create_geometry_empty(struct allocator alloc)
{
    struct geometry new_geometry = {
            .name = range_create_dynamic(alloc, sizeof(&new_geometry.name->data), 64),
            .vertices = range_create_dynamic(alloc, sizeof(&new_geometry.vertices->data), 128),
    };

    return new_geometry;
}

/**
 * @brief
 *
 * @param alloc
 * @param geometry
 */
void destroy_geometry(struct allocator alloc, struct geometry *geometry)
{
    if (!geometry) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->name));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(geometry->vertices));

    *geometry = (struct geometry) { 0 };
}
