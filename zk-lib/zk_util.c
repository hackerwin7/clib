//
// Created by fff on 3/6/16.
// implement the zk_utl.h, add reconnect and retry for zk zpi with C

#include "zk_util.h"

/**
 * create the memory for zk handler, the function would not be responsible for mem free
 * @param zk_conn_str zookeeper connection string such as 172.17.17.17:2181,172.17.17.18:2181
 * @return zhandle_t *
 */
zhandle_t * init_zk_conn(char * zk_conn_str) {
    /* param check */
    if(!zk_conn_str)
        return NULL;
    /*main process for zk creating handler*/
    zhandle_t * zpt = NULL;
    zpt = zookeeper_init(zk_conn_str, 0, Z_TIMEOUT, 0, 0, 0);
    /*return check*/
    if(!zpt)
        return NULL;
    return zpt;
}

/**
 * get the node data by node path
 * @zk_handle zk_handle that have been initialized
 * @zk_path
 * @get_data
 * @return status of return value
 *      such as ZOO_OK nothing wrong it's OK
 *              ZOO_ERROR something wrong in the function
 *              etc.
 */
int zk_get(zhandle_t * zk_handle, char * zk_path, char * get_data) {
    if(!zk_handle || !zk_path || !get_data)
        return ZOO_PARAM_NULL;
    int buff_len = 1000;
    int ret = zoo_get(zk_handle, zk_path, 0, get_data, &buff_len, NULL);
    get_data[buff_len] = '\0';//set terminated to string
    if(ret == ZOK)
        return ZOO_OK;
    else
        return ZOO_ERROR;
}

/**
 * judge the zookeeper node is exists or not
 * @param zk_handle
 * @path
 * @return 1 -> exists, 0 -> not exists
 */
int zk_is_exists(zhandle_t * zk_handle, char * path) {
    if(!zk_handle || !path)
        return ZOO_PARAM_NULL;
    int ret = zoo_exists(zk_handle, path, 0, NULL);
    if(ret == ZOK)
        return 1;
    else //ZNONODE etc.
        return 0;
}

/**
 * create the zk node and node data
 * @param zk_handle
 * @zk_path
 * @zk_data
 * @return status
 */
int zk_create(zhandle_t * zk_handle, char * zk_path, char * zk_data) {
    if(!zk_handle || !zk_path)
        return ZOO_PARAM_NULL;
    if(!zk_is_exists(zk_handle, zk_path)) {
        int ret = zoo_create(zk_handle
                , zk_path, zk_data, (int) strlen(zk_data)
                , &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
        if(ret == ZOK)
            return ZOO_OK;
        else
            return ZOO_ERROR;
    } else
        return ZOO_OK;
}

/**
 * set the data of the zk node
 * @param zk_handle
 * @zk_path
 * @zk_data
 * @return status
 */
int zk_set(zhandle_t * zk_handle, char * zk_path, char * zk_data) {
    if(!zk_handle || !zk_path)
        return ZOO_PARAM_NULL;
    int ret = zoo_set(zk_handle, zk_path, zk_data, (int) strlen(zk_data), -1);
    if(ret == ZOK)
        return ZOO_OK;
    else
        return ZOO_ERROR;
}

/**
 * delete the zk node (path) and node data
 * @param zk_handle
 * @param path
 * @return status
 */
int zk_delete(zhandle_t * zk_handle, char * path) {
    if(!zk_handle || !path)
        return ZOO_PARAM_NULL;
    int ret= zoo_delete(zk_handle, path, -1);
    if(ret == ZOK)
        return ZOO_OK;
    else
        return ZOO_ERROR;
}

/**
 * get the children node name of the node path
 * @param zk_handle
 * @param path
 * @param nodes the node name struct (data array and length)
 * @return status
 */
int zk_get_children(zhandle_t * zk_handle, char * path, struct String_vector * nodes) {
    if(!zk_handle || !path || !nodes)
        return ZOO_PARAM_NULL;
    int ret = zoo_get_children(zk_handle, path, 0, nodes);
    return ret == ZOK ? ZOO_OK : ZOO_ERROR;
}

/**
 * close the zk handle
 * @param zk
 * @return status
 */
int close_zk_conn(zhandle_t * zk_handle) {
    if(!zk_handle)
        return ZOO_OK;
    int ret = zookeeper_close(zk_handle);
    return ret == ZOK ? ZOO_OK : ZOO_ERROR;
}