
#include "3dful_core.h"

/**
 * @brief Overflow-checked user increment.
 *
 * @param obj
 */
void loadable_add_user(struct loadable *obj)
{
    if (obj->nb_users < UINT16_MAX) {
        obj->nb_users += 1;
    }
}

/**
 * @brief Overflow-checked user decrement.
 *
 * @param obj
 */
void loadable_remove_user(struct loadable *obj)
{
    if (obj->nb_users > 0) {
        obj->nb_users -= 1;
    }
}

/**
 * @brief
 *
 * @param obj
 * @return i32
 */
i32 loadable_needs_loading(struct loadable *obj)
{
    return (obj->nb_users > 0) && (!(obj->flags & LOADABLE_FLAG_LOADED));
}

/**
 * @brief
 *
 * @param obj
 * @return i32
 */
i32 loadable_needs_unloading(struct loadable *obj)
{
    return (obj->nb_users == 0) && (obj->flags & LOADABLE_FLAG_LOADED);
}