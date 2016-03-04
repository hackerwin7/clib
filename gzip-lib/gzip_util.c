//
// Created by fff on 3/4/16.
//
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "gzip_util.h"

#define CHUNK 16384
#define COMPRESSION_LEVEL 6

/**
 * create gzip data by string
 * @param msg
 */
gzip_datap gzip_data_create(char * msg) {
    size_t len = strlen(msg);
    gzip_datap pt = (gzip_datap)malloc(sizeof(gzip_data));
    memcpy(pt->data, msg, len);
    pt->len = len;
    return pt;
}

/**
 * free gzip data
 * @param pt
 */
void gzip_data_free(gzip_datap pt) {
    if(pt) {
        free(pt);
        pt = NULL;
    }
}

/**
 * compress the data use deflate
 * @param src
 * @param des
 * @return -1 is failure
 * ??????? problem : how to GC the out_data or return char * ???
 * malloc and free memory op , we should try to place it outside not inner code
 */
int gzip_compress(gzip_datap src, gzip_datap des) {
    if(src == NULL || des == NULL ||
            src->len <=0 || des->len <= 0)
        return -1;
    /* init */
    size_t slen = src->len; //buffer length can not use strlen !!!!!!!!!!!!!!!!! unless the raw data is string (null-terminated)
    int sig = 0;//return signal code (Z_OK is ok)
    int flush = 0;// multiple CHUNk block
    unsigned have = 0;
    z_streamp streamp = (z_streamp)malloc(sizeof(z_stream));
    unsigned char in[CHUNK], out[CHUNK];
    streamp->zalloc = Z_NULL;
    streamp->zfree = Z_NULL;
    streamp->opaque = Z_NULL;
    sig = deflateInit(streamp, COMPRESSION_LEVEL);
    if(sig != Z_OK)//init failure
        return -1;

    /* compress config */
    size_t inps = 0;
    size_t outps = 0;
    do {
        if(slen > CHUNK) {
            memcpy(in, src->data + inps, CHUNK);//operate pointer not char array
            slen -= CHUNK;
            inps += CHUNK;
            flush = Z_NO_FLUSH;
            streamp->avail_in = CHUNK;
            streamp->next_in = in;
        } else {
            //slen must > 0 (see previous if condition and slen -= CHUNK)
            memcpy(in, src->data + inps, slen);
            flush = Z_FINISH;;
            streamp->avail_in = (uInt)slen;
            streamp->next_in = in;
        }
        /* compress processing */
        do {
            streamp->avail_out = CHUNK;
            streamp->next_out = out;
            sig = deflate(streamp, flush);
            assert(sig != Z_STREAM_ERROR);
            have = CHUNK - streamp->avail_out;
            memcpy(des->data + outps, out, have); //write out out_data
            outps += have;
        } while (streamp->avail_out == 0);
        assert(streamp->avail_in == 0);
    } while (flush != Z_FINISH);
    assert(sig == Z_STREAM_END);

    /* wrapper des */
    des->len = outps;

    /* clean the malloc memory */
    (void) deflateEnd(streamp);
    return 0;
}

/**
 * decompress data
 * @param src
 * @param des
 */
int gzip_decompress(gzip_datap src, gzip_datap des) {
    /* init */
    size_t slen = src->len;
    printf("init decompress len = %zu\n", slen);
    int sig = 0;
    unsigned have = 0;
    z_streamp streamp = (z_streamp)malloc(sizeof(z_stream));
    unsigned char in[CHUNK], out[CHUNK];
    streamp->zalloc = Z_NULL;
    streamp->zfree = Z_NULL;
    streamp->opaque = Z_NULL;
    streamp->avail_in = 0;
    streamp->next_in = Z_NULL;
    sig = inflateInit(streamp);
    if(sig != Z_OK)
        return -1;

    /* decompress config */
    size_t inps = 0;
    size_t outps = 0;
    do {
        if(slen > CHUNK) {
            memcpy(in, src->data + inps, CHUNK);
            slen -= CHUNK;
            inps += CHUNK;
            streamp->avail_in = CHUNK;
            streamp->next_in = in;
        } else {
            memcpy(in, src->data + inps, slen);
            streamp->avail_in = (uInt)slen;
            streamp->next_in = in;
        }
        if(streamp->avail_in == 0)
            break;
        /* compress processing */
        do {
            streamp->avail_out = CHUNK;
            streamp->next_out = out;
            sig = inflate(streamp, Z_NO_FLUSH);
            assert(sig != Z_STREAM_ERROR);
            switch (sig) {
                case Z_NEED_DICT:
                    sig = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(streamp);
                    return -1;
            }
            have = CHUNK - streamp->avail_out;
            memcpy(des->data + outps, out, have);// write data
            outps += have;
        } while (streamp->avail_out == 0);
    } while (sig != Z_STREAM_END);

    /* wrapper des */
    des->len = outps;

    /* clean the malloc memory */
    (void) inflateEnd(streamp);
    return 0;
}