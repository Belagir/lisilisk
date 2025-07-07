
#include <stdio.h>

#include "file_operations.h"

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
i32 file_read(const char *path, BUFFER *out_buffer)
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

    read_length = fread(out_buffer->data, 1, out_buffer->capacity, fd);
    out_buffer->length = read_length;

    fclose(fd);
    return 0;
}
