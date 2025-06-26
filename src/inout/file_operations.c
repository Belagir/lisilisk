
#include <stdio.h>

#include "file_operations.h"

/**
 * @brief
 *
 * @param path
 * @param out_buffer
 */
i32 file_read(const char *path, BUFFER *out_buffer)
{
    FILE *fd = 0;
    size_t length = 0u;
    i32 err_code = 0;

    if (!path || !out_buffer) {
        err_code = -1;
        goto cleanup_on_error;
    }

    fd = fopen(path, "r");
    if (!fd) {
        err_code = -2;
        goto cleanup_on_error;
    }

    fseek(fd, 0, SEEK_END);
    length = ftell(fd);

    if (out_buffer->capacity < (length + 1)) {
        err_code = -3;
        goto cleanup_on_error;
    }

    fseek(fd, 0, SEEK_SET);
    fread(out_buffer->data, length, 1, fd);
    out_buffer->length = length;
    range_push(RANGE_TO_ANY(out_buffer), &(char) {'\0'});

cleanup_on_error:
    fclose(fd);
    return err_code;
}
