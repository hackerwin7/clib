//
// Created by fff on 3/17/16.
//

#ifndef CLIB_KAFKA_UTIL_H
#define CLIB_KAFKA_UTIL_H

#endif //CLIB_KAFKA_UTIL_H

#include "librdkafka/rdkafka.h"

/* kafka producer handler */
typedef struct u_rdkafka_producer_s {
    rd_kafka_t * rk;
    rd_kafka_conf_t * rk_conf;
    rd_kafka_topic_t * rk_topic[100]; //max 100 topics
    rd_kafka_topic_conf_t * rk_topic_conf;// only one topic conf
    char err_msg[1024];
    char broker_list[1024];
    char topics[100][64];
    int topic_len;
} u_rdkafka_producer, *u_rdkafka_producerp;


/* function header */
u_rdkafka_producerp u_rdkafka_producer_init(const char *brokers);

int u_rdkafka_config_set(u_rdkafka_producerp handle, const char *name, const char *value);
int u_rdkafka_config_topic_set(u_rdkafka_producerp handle, const char *name, const char *value);

int u_rdkafka_add_topic(u_rdkafka_producerp handle, const char * topic);

int u_rdkafka_send(u_rdkafka_producerp handler, const char * topic, void * payload, size_t paylen, void * key, size_t keylen);
int u_rdkafka_producer_close(u_rdkafka_producerp handler);