//
// Created by fff on 3/4/16.
//

#ifndef CLIB_GZIP_UTIL_H
#define CLIB_GZIP_UTIL_H

#endif //CLIB_GZIP_UTIL_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "zlib/zlib.h"

#define MAX_BUFFER_LEN 20000

/* struct */
typedef struct gzip_data {
    unsigned char data[MAX_BUFFER_LEN];
    size_t len;
}gzip_data, * gzip_datap;

/* create gzip data by string
 * @param the source data is string
 * @return gzip struct data that can be used with gzip util
 */
gzip_datap gzip_data_create_str(char *msg);

/**
 * if the source is not string but a raw data, you can use this create function
 * @param msg
 * @param len
 * @return gzip struct data
 */
gzip_datap gzip_data_create(unsigned char * msg, size_t len);

/* free gzip data */
void gzip_data_free(gzip_datap pt);

/* compress data */
int gzip_compress(gzip_datap src, gzip_datap des);

/* uncompress data */
int gzip_decompress(gzip_datap src, gzip_datap des);