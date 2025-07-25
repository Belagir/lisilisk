/**
 * @file file_operations.c
 * @author Gabriel Bédat
 * @brief Implements file I/O procedures.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "file_operations.h"

#include <stdio.h>

#include <ustd_impl/array_impl.h>

/**
 * @brief Reads a file, count the number of bytes to the end, and returns this
 * value. Returns 0 bytes if the file cannot be read, for any reason.
 *
 * @param[in] path OS-compliant path to the file.
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
 * @brief Reads a file, copying byte-by-byte its contents into a buffer.
 *
 * @param[in] path OS-compliant path to the file.
 * @param[out] out_buffer Pointer to some valid memory of at least
 * out_buffer_cap bytes.
 * @param[in] out_buffer_cap Size, in bytes, of the supplied buffer.
 * @param[out] nb_read_bytes Optional pointer to some integer that will
 * be filled by the number of bytes read.
 * @return i32 (see  FILE_OP_* defines)
 */
i32 file_read(const char *path, byte *out_buffer, size_t out_buffer_cap,
        size_t *nb_read_bytes)
{
    FILE *fd = 0;
    size_t read_length = 0;

    if (!path || !out_buffer) {
        return FILE_OP_CANNOT_WORK;
    }

    fd = fopen(path, "r");
    if (!fd) {
        return FILE_OP_OPEN_FAILED;
    }

    read_length = fread(out_buffer, 1, out_buffer_cap, fd);
    if (nb_read_bytes) *nb_read_bytes = read_length;

    fclose(fd);
    return FILE_OP_OK;
}

/**
 * @brief Like file_read(), but uses an array made with ustd/array.h.
 *
 * @param path OS-compliant path to the file.
 * @param out_array valid array able to hold the content of the file.
 * @return i32 (see  FILE_OP_* defines)
 */
i32 file_read_to_array(const char *path, byte *out_array)
{
    struct array_impl *target_array = nullptr;

    if (!out_array) {
        return FILE_OP_CANNOT_WORK;
    }

    target_array = array_impl_of(out_array);

    return file_read(path, out_array, target_array->capacity,
            &target_array->length);
}
