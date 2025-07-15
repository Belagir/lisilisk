
#include "file_operations.h"

#include <stdio.h>

#include <ustd_impl/array_impl.h>

/**
 * @brief
 *
 * @param path
 * @return size_t
 */
size_t file_length(const char *path)
{
    FILE *fd = 0;
    size_t length = 0u;

    if (!path) {
        return 0;
    }

    fd = fopen(path, "r");
    if (!fd) {
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    length = ftell(fd);
    fclose(fd);

    return length;
}

/**
 * @brief
 *
 * @param path
 * @param out_buffer
 */
i32 file_read(const char *path, byte *out_buffer, size_t out_buffer_cap, size_t *nb_read_bytes)
{
    FILE *fd = 0;
    size_t read_length = 0;

    if (!path || !out_buffer) {
        return -1;
    }

    fd = fopen(path, "r");
    if (!fd) {
        return -2;
    }

    read_length = fread(out_buffer, 1, out_buffer_cap, fd);
    if (nb_read_bytes) *nb_read_bytes = read_length;

    fclose(fd);
    return 0;
}

/**
 * @brief
 *
 * @param path
 * @param out_array
 * @return i32
 */
i32 file_read_to_array(const char *path, byte *out_array)
{
    struct array_impl *target_array = array_impl_of(out_array);

    return file_read(path, out_array, target_array->capacity, &target_array->length);
}
