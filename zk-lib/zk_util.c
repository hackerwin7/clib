//
// Created by fff on 3/2/16.
// desc :
//      wrap the zookeeper.h apply some interface for zookeeper operation
//
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

#include "zookeeper/zookeeper.h"
#include "zk_util.h"

#define MALLOC_DEFAULT_LEN 500

/* const var */
static const int Z_TIMEOUT = 30000;
static const int T_SLEEP_UNIT = 6000;
static const int C_RECONNECT_CNT = 10;
static const int C_RETRY_CNT = 3;
static const int T_SLEEP_RETRY_UNIT = 1000;

/* internal function statement */
static int s_init_zk_conn(char * conn_str);
static int s_handler_is_null();
static void s_handler_close();
static int s_handler_get_children(char *path, struct String_vector *strings);
static int s_handler_get(char *path, char * buffer, int * buffer_len);
static int s_handler_exists(char * path);

/* internal zookeeper handler */
static zhandle_t * zk_handler_t = NULL;

/* global buffer */
char buffer[MALLOC_DEFAULT_LEN] = {'\0'};

/**
 * originally init the zk
 * @ str zk conn str
 */
int s_init_zk_conn(char * str) {
    zk_handler_t = zookeeper_init(str, 0, Z_TIMEOUT, 0, 0, 0);
    if(!zk_handler_t)
        return 0;
    else
        return 1;
}

/**
 * judge the zk handler is empty or not
 * @return boolean, 1/null 0/not null
 */
int s_handler_is_null() {
    if(!zk_handler_t) return 1;
    else return 0;
}

/**
 * close the zk handler
 */
void s_handler_close() {
    if(zk_handler_t) {
        zookeeper_close(zk_handler_t);
        zk_handler_t = NULL;
    }
}

/**
 * create node with zk handler
 * @param path
 * @param data
 * @return signal (ZOK is success, other is failed)
 */
int s_create_node(char *path, char *data) {
    return zoo_create(zk_handler_t, path, data, (int)strlen(data), &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
}

/**
 * delete the zk node
 * @return delete signal
 */
int s_delete_node(char * path) {
    return zoo_delete(zk_handler_t, path, -1);
}

/**
 * set the data of node
 * @param path
 * @param data
 */
int s_set_node(char * path, char * data) {
    return zoo_set(zk_handler_t, path, data, (int)strlen(data), -1);
}

/**
 * get the children node name
 * @param path node path
 * @param ret_children a char[][] for node name
 * @ret_len the length of list of children
 */
int s_handler_get_children(char *path, struct String_vector *children) {
    return zoo_get_children(zk_handler_t, path, 0, children);
}

/**
 * get node data
 * @param path
 * @param buffer save the getting node data
 * @param buffer_len save the length of buffer_len
 */
int s_handler_get(char * path, char * buffer, int * buffer_len) {
    return zoo_get(zk_handler_t, path, 0, buffer, buffer_len, NULL);
}

/**
 * zk node exists
 * @param path
 */
int s_handler_exists(char * path) {
    return zoo_exists(zk_handler_t, path, 0, NULL);
}

/**
 * init the zk connection
 * @param zk_conn_str : zookeeper connection string
 */
void init_zk_conn(char * zk_conn_str) {
    if(!s_handler_is_null())
        s_handler_close();
    int sig = s_init_zk_conn(zk_conn_str);
    if(!sig) { //reconnect for loops
        int count = 0;
        while (++count <= C_RECONNECT_CNT && !sig) {
            sig = s_init_zk_conn(zk_conn_str);
            usleep(T_SLEEP_UNIT);
        }
    }
    if(!sig) { //connection error
        fprintf(stderr, "zookeeper connection error!!!");
        exit(-1);
    }
}

/**
 * close the zookeeper connection
 */
void close_zk_conn() {
    s_handler_close();
}

/**
 * get path node data
 * @param zk_path
 * @return if OK return data else return NULL
 */
char * zk_get(char * zk_path) {
    int buffer_len = MALLOC_DEFAULT_LEN;
    int sig = s_handler_get(zk_path, buffer, &buffer_len);
    if(sig == ZOK)
        return buffer;
    else {
        int count = 0;
        while (++count <= C_RETRY_CNT && sig != ZOK) { // retry
            sig = s_handler_get(zk_path, buffer, &buffer_len);
            usleep(T_SLEEP_RETRY_UNIT);
        }
        return NULL;//retry failed
    }
}

/**
 * zk exists
 * @param path
 */
int zk_is_exists(char * path) {
    int sig = s_handler_exists(path);
    if(sig == ZOK)
        return 1;
    else
        return 0;
}

/**
 * zk set recursive set and create
 * @param zk_path
 * @param zk_data
 *
 */
void zk_set(char * zk_path, char * zk_data) {
    int sig = s_set_node(zk_path, zk_data);
    if(sig != ZOK) {
        int count = 0;
        while (++count <= C_RETRY_CNT && sig != ZOK) {
            sig = s_set_node(zk_path, zk_data);
            usleep(T_SLEEP_RETRY_UNIT);
        }
    }
}

/**
 * create zk node
 * @param zk_path
 * @param zk_data
 */
void zk_create(char * zk_path, char * zk_data) {
    if(!zk_is_exists(zk_path))
        s_create_node(zk_path, zk_data);
}

/**
 * delete the zk node
 * @param path
 */
void zk_delete(char *path) {
    s_delete_node(path);
}

/**
 * get node list
 * @param path
 * @param nodes
 */
void zk_get_children(char * path, struct String_vector * nodes) {
    s_handler_get_children(path, nodes);
}
