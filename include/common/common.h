//
// Created by fff on 3/4/16.
//

#include <stddef.h>

#ifndef CLIB_COMMON_H
#define CLIB_COMMON_H

#endif //CLIB_COMMON_H

/* raw bytes */
typedef struct c_byte_buffer_s {
    unsigned char * data;//free first this data, then free itself
    size_t len;
} c_byte_buffer, * c_byte_bufferp;