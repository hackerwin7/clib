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

#define MAX_BUFFER_LEN 100000

/* struct */
typedef struct gzip_data {
    unsigned char data[MAX_BUFFER_LEN];
    size_t len;
}gzip_data, * gzip_datap;

/* create gzip data by string */
gzip_datap gzip_data_create(char * msg);

/* free gzip data */
void gzip_data_free(gzip_datap pt);

/* compress data */
int gzip_compress(gzip_datap src, gzip_datap des);

/* uncompress data */
int gzip_decompress(gzip_datap src, gzip_datap des);