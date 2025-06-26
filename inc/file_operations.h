
#include <ustd/range.h>

typedef RANGE(byte) BUFFER;

i32 file_read(const char *path, BUFFER *out_buffer);
