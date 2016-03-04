//
// Created by fff on 3/2/16.
//
#include <stdio.h>
#include "stdlib.h"
#include "zookeeper/zookeeper.h"

#ifndef CLIB_ZK_UTIL_H
#define CLIB_ZK_UTIL_H

#endif //CLIB_ZK_UTIL_H

/**
 * initialize the zookeeper connection with zookeeper string
 */
void init_zk_conn(char * zk_conn_str);

/**
 * zk get
 */
char * zk_get(char * zk_path);

/* create node */
void zk_create(char * zk_path, char * zk_data);

/**
 * zk set and create (if not exists)
 */
void zk_set(char * zk_path, char * zk_data);

/**
 * zookeeper exists path
 */
int zk_is_exists(char * path);

/* delete zk node */
void zk_delete(char *path);

/* get children */
void zk_get_children(char *path, struct String_vector * nodes);

/* close zk connection */
void close_zk_conn();