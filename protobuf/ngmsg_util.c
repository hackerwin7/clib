//
// Created by fff on 3/11/16.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"
#include "stdint.h"

#include "ngmsg_util.h"

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
    msg.ip = (char *) malloc(ip_len);
    memcpy(msg.ip, ip, ip_len);
    msg.d = (char *) malloc(d_len);
    memcpy(msg.d, d, d_len);
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
    void * buff = malloc(len);
    ngmsg__pack(&msg, buff);
    buffer->len = len;
    buffer->data = malloc(len);
    memcpy(buffer->data, buff, len);
    return 0;
}

/**
 * deserialize from the buffer bytes
 * @param buffer
 * @param msgp
 * @return status
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

/**
 * transfer the bytes for the lua call
 * @param c
 * @param rtm
 * @param ip
 * @param d
 * @return bytes
 */
c_byte_buffer pb_tramsfer(int c, int64_t rtm, const char * ip, const char * d) {
    NGmsg msg = pb_build_ng_msg(c, rtm, ip, d);
    c_byte_bufferp buff = c_byte_buffer_create();
    pb_serialize_ng_msg(msg, buff);
    return *buff;
}

/*
 * de-trasfer for the lua call
 * @param buffer
 * @param msgp
 * @return status
 */
int pb_de_transfer(c_byte_buffer buffer, ngmsgp msgp) {
    NGmsg ngmsg;
    pb_deserialize_ng_msg(&buffer, &ngmsg);
    msgp->c = ngmsg.c;
    msgp->rtm = ngmsg.rtm;
    strcpy(msgp->ip, ngmsg.ip);
    strcpy(msgp->d, ngmsg.d);
    return 0;
}