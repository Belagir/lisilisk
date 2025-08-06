/**
 * @file file_operations.h
 * @author Gabriel Bédat
 * @brief Aggregates functions and procedures about stream I/O.
 * @version 0.1
 * @date 2025-07-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef FILE_OPERATIONS_H__
#define FILE_OPERATIONS_H__

#include <ustd/common.h>

/** When no error occured. */
#define FILE_OP_OK           (0)
/** When the file could not be opened (not found or the program has not the
  * right permissions). */
#define FILE_OP_OPEN_FAILED (-1)
/** A procedure complains about being passed invalid arguments. */
#define FILE_OP_CANNOT_WORK (-2)

// Reads the length, in bytes of some file.
size_t file_length(const char *path);
// Reads a file to a buffer, returning 0 if successful.
i32    file_read(const char *path, byte *out_buffer, size_t out_buffer_cap,
        size_t *nb_read_bytes);
// Reads a file to an array (created with ustd/array.h), returning 0
// if successful.
i32    file_read_to_array(const char *path, byte *out_array);

#endif
