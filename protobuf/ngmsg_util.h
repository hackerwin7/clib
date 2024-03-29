//
// Created by fff on 3/11/16.
//

#ifndef CLIB_NGMSG_UTIL_H
#define CLIB_NGMSG_UTIL_H

#endif //CLIB_NGMSG_UTIL_H

#include "ngmsg.pb-c.h"
#include "common.h"

/* build the msg from the param */
NGmsg pb_build_ng_msg(int c, int64_t rtm, const char * ip, const char * d);

/* serialize the msg to bytes buffer */
int pb_serialize_ng_msg(NGmsg msg, c_byte_bufferp buffer);

/* deserialize the bytes buffer to the msg */
int pb_deserialize_ng_msg(c_byte_bufferp buffer, NGmsg* msgp);