
#ifndef FILE_OPERATIONS_H__
#define FILE_OPERATIONS_H__

#include <ustd/range.h>

typedef RANGE(byte) BUFFER;

size_t file_length(const char *path);
i32    file_read(const char *path, BUFFER *out_buffer);

#endif
