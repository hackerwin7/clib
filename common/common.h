//
// Created by fff on 3/4/16.
//

#include <stddef.h>

#ifndef CLIB_COMMON_H
#define CLIB_COMMON_H

#endif //CLIB_COMMON_H

/* raw bytes */
typedef struct c_byte_buffer_s {
    void * data;//free first this data, then free itself
    size_t len;//the length of bytes
} c_byte_buffer, * c_byte_bufferp;

/* build the memory for the struct only */
c_byte_bufferp c_byte_buffer_create();

/* with len to build */
c_byte_bufferp c_byte_buffer_create_size(size_t size);

/* create with string */
c_byte_bufferp c_byte_buffer_create_str(const char * str);

/* copy from bytes */
c_byte_bufferp c_byte_buffer_create_cpy(void * buff, size_t len);

/* free */
void c_byte_buffer_free(c_byte_bufferp p);