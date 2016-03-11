//
// Created by fff on 3/11/16.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "common.h"

/**
 * create a memory for struct only
 * return byte buffer
 */
c_byte_bufferp c_byte_buffer_create() {
    c_byte_bufferp p = (c_byte_bufferp) malloc(sizeof(c_byte_buffer));
    return p;
}

/**
 * create a byte buffer with size
 * @param size
 * @return bytes
 */
c_byte_bufferp c_byte_buffer_create_size(size_t size) {
    c_byte_bufferp p = (c_byte_bufferp) malloc(sizeof(c_byte_buffer));
    p->len = size;
    p->data = malloc(size);
    return p;
}

/**
 * create a byte buffer with string
 * @param str
 * @return bytes
 */
c_byte_bufferp c_byte_buffer_create_str(const char * str) {
    size_t para_len = strlen(str);
    c_byte_bufferp p = (c_byte_bufferp) malloc(sizeof(c_byte_buffer));
    p->len = para_len;
    p->data = malloc(para_len);
    memcpy(p->data, str, para_len);
    return p;
}

/**
 * copy from the another bytes
 * @param buff
 * @param len
 * @return bytes
 */
c_byte_bufferp c_byte_buffer_create_cpy(void * buff, size_t len) {
    c_byte_bufferp p = (c_byte_bufferp) malloc(sizeof(c_byte_buffer));
    p->len = len;
    p->data = malloc(len);
    memcpy(p->data, buff, len);
    return p;
}