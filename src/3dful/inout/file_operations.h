
#ifndef FILE_OPERATIONS_H__
#define FILE_OPERATIONS_H__

#include <ustd/common.h>

size_t file_length(const char *path);
i32    file_read(const char *path, byte *out_buffer, size_t out_buffer_cap, size_t *nb_read_bytes);
i32    file_read_to_array(const char *path, byte *out_array);

#endif
