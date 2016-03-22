//
// Created by fff on 3/21/16.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "netinet/in.h"
#include "net/if.h"
#include "arpa/inet.h"
#include "unistd.h"

#include "kafka_util.h"
#include "m_librdkafka.h"

/* send monitor for librdkafka */
const char * monitor_brokers = "localhost:9092";
const char * monitor_topic = "monitor";
u_rdkafka_producerp monitor = NULL;

/**
 * get local machine ip
 * @return ip
 */
char *m_get_local_ip() {
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "enp0s25", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    char * ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    if(strcmp(ip, "0.0.0.0") == 0) {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
        ioctl(fd, SIOCGIFADDR, &ifr);
        close(fd);
        ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    }
    return ip;
}

/**
 * send monitor with cluster status
 * format sample : ip#{ "cluster": "192.168.147.109:9092",”status”:0,”t”:147526984578}
 * @param clusters
 * @param status
 * @param ts
 */
void send_cluster_avai_monitor(char *clusters, int status, long ts) {
    /* init the monitor handler */
    if(!monitor) {
        monitor = u_rdkafka_producer_init(monitor_brokers);
        u_rdkafka_add_topic(monitor, monitor_topic);
    }
    /* build monitor msg */
    char * msg = (char *) malloc(sizeof(char) * (strlen(clusters) + 512));
    memset(msg, '\0', sizeof(msg));
    char * ip = m_get_local_ip();
    sprintf(msg, "%s#{ \"cluster\": \"%s\",\"status\":%d,\"t\":%ld}", ip, clusters, status, ts);
    /* send monitor */
    u_rdkafka_send(monitor, monitor_topic, msg, strlen(msg), NULL, 0);
    /* free */
    free(msg);
    msg = NULL;
}

/**
 * send the statistics of the status to the monitor
 * format sample : ip#json_status_string
 * @param stats
 */
void send_producer_status_monitor(char * stats) {
    //init monitor
    if(!monitor) {
        monitor = u_rdkafka_producer_init();
        u_rdkafka_producer_connect(monitor, monitor_brokers);
        u_rdkafka_add_topic(monitor, monitor_topic);
    }
    //build monitor msg
    char * msg = NULL;
    char * ip = m_get_local_ip();
    size_t msg_len = strlen(stats) + strlen(ip) + 10;
    msg = (char *) malloc(sizeof(char) * msg_len);
    memset(msg, '\0', sizeof(msg));
    strcpy(msg, ip);
    strcat(msg, "#");
    strcat(msg, stats);
    //send monitor
    u_rdkafka_send(monitor, monitor_topic, msg, strlen(msg), NULL, 0);
    //free mem
    free(msg);
    msg = NULL;
}