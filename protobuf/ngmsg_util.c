//
// Created by fff on 3/11/16.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"
#include "stdint.h"

#include "ngmsg_util.h"
#include "gc_ring_buffer.h"

/**
 * build the ng msg
 * @param c
 * @param rtm
 * @param ip
 * @param d
 * @return msg
 */
NGmsg pb_build_ng_msg(int c, int64_t rtm, const char * ip, const char * d) {
    size_t ip_len = strlen(ip);
    size_t d_len = strlen(d);
    NGmsg msg = NGMSG__INIT;
    msg.c = c;
    msg.has_c = 1;// save into the buffer and deserialize the data have it
    msg.rtm = rtm;
    msg.has_rtm = 1;
//    msg.ip = (char *) malloc(ip_len);
//    memcpy(msg.ip, ip, ip_len);
//    msg.d = (char *) malloc(d_len);
//    memcpy(msg.d, d, d_len);

    //modify the malloc
    msg.ip = (char *)ip;
    msg.d = (char *)d;
    return msg;
}

/**
 * serialize the msg to bytes
 * @param msg
 * @param buffer bytes
 * @return status
 */
int pb_serialize_ng_msg(NGmsg msg, c_byte_bufferp buffer) {
    size_t  len = ngmsg__get_packed_size(&msg);
    buffer->data = malloc_ring_gc(len);//put into gc ring buffer
    ngmsg__pack(&msg, buffer->data);
    buffer->len = len;
    return 0;
}

/**
 * deserialize from the buffer bytes
 * @param buffer
 * @param msgp
 * @return statuss
 */
int pb_deserialize_ng_msg(c_byte_bufferp buffer, NGmsg* msgp) {
    if (!buffer)
        return -1;
    NGmsg * tmsgp = ngmsg__unpack(NULL, buffer->len, buffer->data);
    if(!tmsgp)
        return -1;
    /* return msgp as the param, do not free the msgp */
    msgp->c = tmsgp->c;
    msgp->rtm = tmsgp->rtm;
    msgp->ip = tmsgp->ip;
    msgp->d = tmsgp->d;
    return 0;
}