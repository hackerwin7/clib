//
// Created by fff on 3/2/16.
// zk utils interface statement
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "zookeeper/zookeeper.h"

#ifndef CLIB_ZK_UTIL_H
#define CLIB_ZK_UTIL_H

#endif //CLIB_ZK_UTIL_H

/* const value */
#define ZOO_OK 0
#define ZOO_ERROR -1
#define ZOO_PARAM_NULL -2
#define ZOO_PARAM_ERROR -5
#define ZOO_HANDLE_NULL -6
#define ZOO_PROC_ERROR -7

#define Z_TIMEOUT 30000

/**
 * initialize the zookeeper connection with zookeeper string
 * @param zk_conn_str zookeeper connection string such as 172.17.17.17:2181,172.17.17.18:2181
 * @return zhandle_t *
 */
zhandle_t * init_zk_conn(char * zk_conn_str);

/**
 * zk getter
 * @zk_handle zk_handle that have been initialized
 * @zk_path
 * @get_data
 * @return status of return value
 *      such as ZOO_OK nothing wrong it's OK
 *              ZOO_ERROR something wrong in the function
 *              etc.
 */
int zk_get(zhandle_t * zk_handle, char * zk_path, char * get_data);

/**
 * zookeeper exists path
 * @param zk_handle
 * @path
 * @return 1 -> exists, 0 -> not exists
 */
int zk_is_exists(zhandle_t * zk_handle, char * path);

/**
 * create node
 * @param zk_handle
 * @zk_path
 * @zk_data
 * @return status
 */
int zk_create(zhandle_t * zk_handle, char * zk_path, char * zk_data);

/**
 * zk set and create (if not exists)
 * @param zk_handle
 * @zk_path
 * @zk_data
 * @return status
 */
int zk_set(zhandle_t * zk_handle, char * zk_path, char * zk_data);

/*
 * delete zk node
 * @param zk_handle
 * @param path
 * @return status
 */
int zk_delete(zhandle_t * zk_handle, char * path);

/*
 * get children
 * @param zk_handle
 * @param path
 * @param nodes the node name struct (data array and length)
 * @return status
 */
int zk_get_children(zhandle_t * zk_handle, char * path, struct String_vector * nodes);

/*
 * close zk connection
 * @param zk
 * @return status
 */
int close_zk_conn(zhandle_t * zk_handle);