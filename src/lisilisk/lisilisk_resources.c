
#include "lisilisk_internals.h"

#include <fts.h>

/**
 * @brief
 *
 * @param folder
 * @param res_manager
 */
void lisilisk_populate_resources(
        const char *folder,
        struct resource_manager *res_manager)
{
    (void) res_manager;

    FTS *hierarchy_stream = fts_open((char *const [])
        {(char *const) folder, nullptr }, FTS_LOGICAL, nullptr);

    FTSENT *entry = nullptr;
    while((entry = fts_read(hierarchy_stream))) {
        if (entry->fts_info == FTS_F) {
            resource_manager_touch(res_manager, "lisilisk", entry->fts_path,
                    make_system_allocator());
        }
    }
}
