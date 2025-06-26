
#include "opengl_scenes.h"

/**
 * @brie
 *
 * @param alloc
 * @return struct object
 */
struct object create_object_empty(struct allocator alloc)
{
    struct object new_object = {
            .name = range_create_dynamic(alloc, sizeof(&new_object.name->data), 64),
            .vertices = range_create_dynamic(alloc, sizeof(&new_object.vertices->data), 128),
    };

    return new_object;
}

/**
 * @brief
 *
 * @param alloc
 * @param object
 */
void destroy_object(struct allocator alloc, struct object *object)
{
    if (!object) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(object->name));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(object->vertices));

    *object = (struct object) { 0 };
}
