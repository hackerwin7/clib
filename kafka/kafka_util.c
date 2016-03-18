//
// Created by fff on 3/17/16.
//

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include <ctype.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>

#include "kafka_util.h"

#define MAX_TOPIC 100

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

void rd_logger(const rd_kafka_t *rkt, int level, const char *fac, const char *buf) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "%u.%03u RDKAFKA-%i-%s: %s: %s\n",
            (int)tv.tv_sec, (int)(tv.tv_usec / 1000),
            level, fac, rkt ? rd_kafka_name(rkt) : NULL, buf);
}

static void msg_delivered2 (rd_kafka_t *rk,
                            const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err)
        fprintf(stderr, "%% Message delivery failed: %s\n",
                rd_kafka_message_errstr(rkmessage));
    else
        fprintf(stderr,
                "%% Message delivered (%zd bytes, offset %"PRId64", "
                        "partition %"PRId32")\n",
                rkmessage->len, rkmessage->offset, rkmessage->partition);
}

/**
 * set the name value for the kafka config
 * @param handle
 * @param name
 * @param value
 * @return status
 */
int u_rdkafka_config_set(u_rdkafka_producerp handle, const char *name, const char *value) {
    rd_kafka_conf_set(handle->rk_conf, name, value, NULL, 0);
    return 0;
}

/**
 * set name value for the kafka topic config
 * @param handle
 * @param name
 * @param value
 * @return status
 */
int u_rdkafka_config_topic_set(u_rdkafka_producerp handle, const char *name, const char *value) {
    rd_kafka_topic_conf_set(handle->rk_topic_conf, name, value, NULL, 0);
    return 0;
}

/**
 * init the kafka handler
 * @return u_rdkafka_producerp
 */
u_rdkafka_producerp u_rdkafka_producer_init(const char *brokers) {
    u_rdkafka_producerp p = (u_rdkafka_producerp) malloc(sizeof(u_rdkafka_producer));
    p->topic_len = 0;
    p->rk_conf = rd_kafka_conf_new();
    p->rk_topic_conf = rd_kafka_topic_conf_new();
    char tmp[128] = {'\0'};
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    u_rdkafka_config_set(p, "internal.termination.signal", tmp);
    u_rdkafka_config_topic_set(p, "produce.offset.report", "true");
    rd_kafka_conf_set_log_cb(p->rk_conf, rd_logger);
    rd_kafka_conf_set_dr_msg_cb(p->rk_conf, msg_delivered2);
    p->rk = rd_kafka_new(RD_KAFKA_PRODUCER, p->rk_conf, p->err_msg, sizeof(p->err_msg));
    if(!p->rk) {
        fprintf(stderr, "%% Failed to create new producer: %s\n", p->err_msg);
        return NULL;
    }
    rd_kafka_set_log_level(p->rk, LOG_DEBUG);
    if(!rd_kafka_brokers_add(p->rk, brokers)) {
        fprintf(stderr, "%% No valid brokers specified\n");
        return NULL;
    }
    strcpy(p->broker_list, brokers);
    return p;
}

/**
 * add a topic to handler
 * @param topic
 * @return status
 */
int u_rdkafka_add_topic(u_rdkafka_producerp handle, const char * topic) {
    if(handle->topic_len >= MAX_TOPIC) {
        sprintf(stderr, "[Err] : the handle reach to the max of the topic length %d, can not add topic.", MAX_TOPIC);
        return -1;
    }
    //re-init the topic conf
    if(!handle->rk_topic_conf) {
        handle->rk_topic_conf = rd_kafka_topic_conf_new();
        u_rdkafka_config_topic_set(handle, "produce.offset.report", "true");
    }
    handle->rk_topic[handle->topic_len] = rd_kafka_topic_new(handle->rk, topic, handle->rk_topic_conf);
    handle->rk_topic_conf = NULL;//owned by topic, fresh the topic config to NULL
    strcpy(handle->topics[handle->topic_len], topic);
    handle->topic_len++;
    return 0;
}

/**
 * send msg to topic
 * @param handler
 * @param topic
 * @param payload
 * @param paylen
 * @param key
 * @param keylen
 * @return status
 */
int u_rdkafka_send(u_rdkafka_producerp handler, const char * topic, void * payload, size_t paylen, void * key, size_t keylen) {
    //find topic to send
    int ti = 0;
    for(; ti <= MAX_TOPIC - 1; ti++) {
        if(strcmp(handler->topics[ti], topic) == 0) {
            break;
        }
    }
    if(ti > MAX_TOPIC - 1) {// no this topic
        fprintf(stderr, "no this topic [%s] in the kafka handler.\n", topic);
    } else {//find this topic
        if(rd_kafka_produce(handler->rk_topic[ti],
                            RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
                            payload, paylen,
                            key, keylen,
                            NULL) == -1) {
            sprintf(stderr, "%% Failed to produce to topic %s partition %i: %s\n",
                    rd_kafka_topic_name(handler->rk_topic[ti]),
                    RD_KAFKA_PARTITION_UA,
                    rd_kafka_err2str(rd_kafka_last_error()));
            rd_kafka_poll(handler->rk, 0);
        }
        rd_kafka_poll(handler->rk, 0);
    }
    return 0;
}

/**
 * close the producer, destory the handler
 * @param handler
 * @return status
 */
int u_rdkafka_producer_close(u_rdkafka_producerp handler) {
    rd_kafka_poll(handler->rk, 0);
    while (rd_kaf)
}