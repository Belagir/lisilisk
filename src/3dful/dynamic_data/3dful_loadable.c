/**
 * @file 3dful_loadable.c
 * @author Gabriel Bédat
 * @brief Loadable data state tracker implementation.
 * Everything related to the struct loadable.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "3dful_dynamic_data.h"

/**
 * @brief Overflow-checked user increment.
 * In case of overflow, the number of user will not increment.
 *
 * @param[inout] obj
 */
void loadable_add_user(struct loadable *obj)
{
    if (obj->nb_users < UINT16_MAX) {
        obj->nb_users += 1;
    }
}

/**
 * @brief Overflow-checked user decrement.
 * In case of underflow (data at 0 users), the number will stay at 0.
 *
 * @param[inout] obj
 */
void loadable_remove_user(struct loadable *obj)
{
    if (obj->nb_users > 0) {
        obj->nb_users -= 1;
    }
}

/**
 * @brief Checks whether or not the object needs to be loaded.
 * i.e, the number of users is more than zero and the object is marked as
 * unloaded.
 *
 * @param[in] obj
 * @return bool
 */
bool loadable_needs_loading(const struct loadable *obj)
{
    return (obj->nb_users > 0) && (!(obj->flags & LOADABLE_FLAG_LOADED));
}

/**
 * @brief Checks whether or not the object needs to be unloaded.
 * i.e, the number of users is zero and the object is marked as loaded.
 *
 * @param[in] obj
 * @return bool
 */
bool loadable_needs_unloading(const struct loadable *obj)
{
    return (obj->nb_users == 0) && (obj->flags & LOADABLE_FLAG_LOADED);
}