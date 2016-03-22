//
// Created by fff on 3/21/16.
//

#ifndef MERCURY_GATEWAY_M_LIBRDKAFKA_H
#define MERCURY_GATEWAY_M_LIBRDKAFKA_H

#endif //MERCURY_GATEWAY_M_LIBRDKAFKA_H

/* send monitor */
void send_cluster_avai_monitor(char *clusters, int status, long ts);
void send_producer_status_monitor(char * stats);