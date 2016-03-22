#include <stdio.h>
#include "string.h"
#include "unistd.h"
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/time.h>
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include <sys/syscall.h>
#include "netinet/in.h"
#include "net/if.h"
#include "arpa/inet.h"
#include "ifaddrs.h"
#include "netdb.h"

#include "zk-lib/zk_util.h"
#include "gzip-lib/gzip_util.h"
#include "base64-lib/base64_util.h"
#include "protobuf/ngmsg_util.h"
#include "common/gc_ring_buffer.h"
#include "librdkafka/rdkafka.h"
#include "kafka/kafka_util.h"
#include "kafka/m_librdkafka.h"
/* declare header */
void t_change_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);

/********************************* for lua call gc or not gc ????? *********************************/
/* zk */
//use default

/* gzip */
int gzip_compress_lu(void * s_bytes, size_t s_len, void * d_bytes, size_t * d_len) {
    gzip_datap src = gzip_data_create_origin(s_bytes, s_len);
    gzip_datap des = gzip_data_create();
    gzip_compress(src, des);
    //return set value param
    memcpy(d_bytes, des->data, des->len);
    *d_len = des->len;
    //free
    gzip_data_free(src);
    gzip_data_free(des);
    return 0;
}

int gzip_de_compress_lu(void * s_bytes, size_t s_len, void * d_bytes, size_t * d_len) {
    gzip_datap src = gzip_data_create_origin(s_bytes, s_len);
    gzip_datap des = gzip_data_create();
    gzip_decompress(src, des);
    //return set value param
    memcpy(d_bytes, des->data, des->len);
    *d_len = des->len;
    //free
    gzip_data_free(src);
    gzip_data_free(des);
    return 0;
}

/* base64 */
//use default

/* protobuf */
int pb_transfer_lu(int c, int64_t rtm, const char *ip, const char *d, c_byte_bufferp buffer) {
    if(buffer && buffer->data)
        c_byte_buffer_free(buffer);//free old mem
    if(!buffer) {
        buffer = c_byte_buffer_create();//only buffer!=NULL data==NULL can pass it
        put_ring_gc(buffer);// put into gc ring buffer
    }
    NGmsg msg = pb_build_ng_msg(c, rtm, ip, d);
    pb_serialize_ng_msg(msg, buffer);
    // c_byte_buffer_free(buffer); //gc will free the mem later
    return 0;
}

//ngmsg struct for lua call
typedef struct pb_msg_s {
    int c;
    int64_t rtm;
    char ip[20];
    char *d;
}pb_msg, *pb_msgp;

int pb_de_transfer_lu(c_byte_bufferp buffer, pb_msgp pb) {
    if(!pb)
        pb = (pb_msgp) malloc_ring_gc(sizeof(pb_msg));
    NGmsg nmsg = NGMSG__INIT;
    pb_deserialize_ng_msg(buffer, &nmsg);
    pb->c = nmsg.c;
    pb->rtm = nmsg.rtm;
    memcpy(pb->ip, nmsg.ip, strlen(nmsg.ip));
    size_t d_len = strlen(nmsg.d);
    if(!pb->d || d_len >= 10000) {
        if(pb->d)
            free(pb->d);
        pb->d = malloc_ring_gc(d_len);
    }
    memcpy(pb->d, nmsg.d, d_len);
    return 0;
}

/********************************* for test *********************************/

int test1() {
    printf("hello world!\n");
    char * msg = "get ddd 123";
    printf("%d\n", (int)strlen(msg));
    printf("%d\n", (int)sizeof(msg));
    return 0;
}

int test2() {
    printf("initing......\n");
    zhandle_t* zpt = init_zk_conn("127.0.0.1:2181");

    struct String_vector * nodes = (struct String_vector *) malloc(sizeof(struct String_vector));
    zk_get_children(zpt, "/", nodes);
    if(nodes == NULL) printf("NULL nodes\n");
    printf("%d\n", nodes->count);
    for(int i = 0; i <= nodes->count - 1; i++)
        printf("%s\n", nodes->data[i]);

    int ret = zk_is_exists(zpt, "/zookeeper");
    if(ret)
        printf("/zookeeper is exists\n");
    else
        printf("/zookeeper is not exists\n");

    ret = zk_is_exists(zpt, "/rs");
    if(ret)
        printf("/rs is exists\n");
    else
        printf("/rs is not exists\n");

    zk_create(zpt, "/rs", "init");

    ret = zk_is_exists(zpt, "/rs");
    if(ret)
        printf("/rs is exists\n");
    else
        printf("/rs is not exists\n");

    char data[100];
    ret = zk_get(zpt, "/rs", data);
    printf("%s\n", data);

    zk_set(zpt, "/rs", "changed");

    char change_data[100];
    ret = zk_get(zpt, "/rs", change_data);
    printf("%s\n", change_data);

    zk_delete(zpt, "/rs");

    ret = zk_is_exists(zpt, "/rs");

    if(ret)
        printf("/rs is exists\n");
    else
        printf("/rs is not exists\n");

    if(nodes) {
        free(nodes);
        nodes = NULL;
    }

    close_zk_conn(zpt);

    return 0;
}

int test3() {
    gzip_datap src, des, udes;
    //src = gzip_data_create_str("hello world");
    src = gzip_data_create_str("我嚓嚓嚓嚓嚓嚓嚓擦擦擦擦擦擦!@#!@$!$  ()dest");
    printf("src len = %zu\n", strlen((char *) src->data));
    des = (gzip_datap) malloc(sizeof(gzip_data));
    udes = (gzip_datap) malloc(sizeof(gzip_data));
    des->len = 1;
    gzip_compress(src, des);
    printf("%zu\n", des->len);
    gzip_decompress(des, udes);
    udes->data[udes->len] = '\0';
    printf("recover str :\n%s\n", udes->data);
    return 0;
}

int test4() {
    char a[50] = "hello world";
    char b[50];
    char c[50];

// deflate
// zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = (uInt)strlen(a)+1; // size of input, string + terminator
    defstream.next_in = (Bytef *)a; // input char array
    defstream.avail_out = (uInt)sizeof(b); // size of output
    defstream.next_out = (Bytef *)b; // output char array
    printf("avail_in = %d\n", defstream.avail_in);

    deflateInit(&defstream, 6);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

// This is one way of getting the size of the output
    printf("Deflated size is: %lu\n", (char*)defstream.next_out - b);
    printf("Deflated size is: %s\n", b);

// inflate
// zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = (uInt)((char*)defstream.next_out - b); // size of input
    infstream.next_in = (Bytef *)b; // input char array
    infstream.avail_out = (uInt)sizeof(c); // size of output
    infstream.next_out = (Bytef *)c; // output char array

    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);

    printf("Inflate:\n%lu\n%s\n", strlen(c), c);

    return 0;
}

int test5() {
    c_byte_bufferp p = NULL;
    printf("%zu\n", sizeof(p));
    p = (c_byte_bufferp) malloc(sizeof(c_byte_buffer));
    printf("%zu\n", sizeof(p));
    p->data = (unsigned char *) malloc(10000);
    printf("%zu\n", sizeof(p));
    p->data = (unsigned char *) malloc(10000 * sizeof(unsigned char));
    printf("%zu\n", sizeof(p));
    unsigned char * temp = p->data;
    printf("%zu\n", sizeof(temp));
    free(p);
    printf("%zu\n", sizeof(temp));
    return 0;
}

int test6() {
    char * t = "asq  @$&*% ( 层哦 的 大 所理解 dfe !!噶尔发额头层 )";
    char des[200], udes[200];
    base64_encode(t, des);
    printf("%s\n", des);
    base64_decode(des, udes);
    printf("%s\n", udes);
    return 0;
}

int t_regist_watch(zhandle_t *zh, const char *path, char *data) {
    zk_get_w(zh, path, data, t_change_watcher, NULL);
    return 0;
}

void t_change_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
    if(state == ZOO_CONNECTED_STATE) {
        if(type == ZOO_CHANGED_EVENT) {
            char data[100];
            t_regist_watch(zh, path, data);
            printf("change event trigger change watcher , the path = %s, data = %s\n", path, data);
        }
    }
}

int test7() {
    printf("hello test7\n");
    zhandle_t * zh = init_zk_conn("127.0.0.1:2181");
    int i = 0;
    char data[500];
    zk_get_w(zh, "/watcher", data, t_change_watcher, NULL);
    printf("init watcher data = %s\n", data);
    while (i++ <= 100) {
        zk_get(zh, "/watcher", data);
        printf("while getting data = %s ......\n", data);
        sleep(3);
    }
}

int test8() {
    gzip_datap src = gzip_data_create_str("ffewfae1123 色他fff!!");
    gzip_datap des = gzip_data_create();
    gzip_datap ude = gzip_data_create();
    gzip_compress(src, des);
    printf("%zu\n", src->len);
    printf("%zu\n", des->len);
    gzip_decompress(des, ude);
    printf("%s\n%zu\n", ude->data, ude->len);
    return 0;
}

int test9() {
    NGmsg msg = pb_build_ng_msg(123, (int64_t)1432700228559, "127.0.0.1", "{\n"
            "\"web-app\": {\n"
            "\"servlet\": [\n"
            "{\n"
            "\"servlet-name\": \"cofaxCDS\",\n"
            "\"servlet-class\": \"org.cofax.cds.CDSServlet\",\n"
            "\"init-param\": {\n"
            "\"configGlossary:installationAt\": \"Philadelphia, PA\",\n"
            "\"configGlossary:adminEmail\": \"ksm@pobox.com\",\n"
            "\"configGlossary:poweredBy\": \"Cofax\",\n"
            "\"configGlossary:poweredByIcon\": \"/images/cofax.gif\",\n"
            "\"configGlossary:staticPath\": \"/content/static\",\n"
            "\"templateProcessorClass\": \"org.cofax.WysiwygTemplate\",\n"
            "\"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\",\n"
            "\"templatePath\": \"templates\",\n"
            "\"templateOverridePath\": \"\",\n"
            "\"defaultListTemplate\": \"listTemplate.htm\",\n"
            "\"defaultFileTemplate\": \"articleTemplate.htm\",\n"
            "\"useJSP\": false,\n"
            "\"jspListTemplate\": \"listTemplate.jsp\",\n"
            "\"jspFileTemplate\": \"articleTemplate.jsp\",\n"
            "\"cachePackageTagsTrack\": 200,\n"
            "\"cachePackageTagsStore\": 200,\n"
            "\"cachePackageTagsRefresh\": 60,\n"
            "\"cacheTemplatesTrack\": 100,\n"
            "\"cacheTemplatesStore\": 50,\n"
            "\"cacheTemplatesRefresh\": 15,\n"
            "\"cachePagesTrack\": 200,\n"
            "\"cachePagesStore\": 100,\n"
            "\"cachePagesRefresh\": 10,\n"
            "\"cachePagesDirtyRead\": 10,\n"
            "\"searchEngineListTemplate\": \"forSearchEnginesList.htm\",\n"
            "\"searchEngineFileTemplate\": \"forSearchEngines.htm\",\n"
            "\"searchEngineRobotsDb\": \"WEB-INF/robots.db\",\n"
            "\"useDataStore\": true,\n"
            "\"dataStoreClass\": \"org.cofax.SqlDataStore\",\n"
            "\"redirectionClass\": \"org.cofax.SqlRedirection\",\n"
            "\"dataStoreName\": \"cofax\",\n"
            "\"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\",\n"
            "\"dataStoreUrl\": \"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon\",\n"
            "\"dataStoreUser\": \"sa\",\n"
            "\"dataStorePassword\": \"dataStoreTestQuery\",\n"
            "\"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\",\n"
            "\"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\",\n"
            "\"dataStoreInitConns\": 10,\n"
            "\"dataStoreMaxConns\": 100,\n"
            "\"dataStoreConnUsageLimit\": 100,\n"
            "\"dataStoreLogLevel\": \"debug\",\n"
            "\"maxUrlLength\": 500\n"
            "}\n"
            "},\n"
            "{\n"
            "\"servlet-name\": \"cofaxEmail\",\n"
            "\"servlet-class\": \"org.cofax.cds.EmailServlet\",\n"
            "\"init-param\": {\n"
            "\"mailHost\": \"mail1\",\n"
            "\"mailHostOverride\": \"mail2\"\n"
            "}\n"
            "},\n"
            "{\n"
            "\"servlet-name\": \"cofaxAdmin\",\n"
            "\"servlet-class\": \"org.cofax.cds.AdminServlet\"\n"
            "},\n"
            "{\n"
            "\"servlet-name\": \"fileServlet\",\n"
            "\"servlet-class\": \"org.cofax.cds.FileServlet\"\n"
            "},\n"
            "{\n"
            "\"servlet-name\": \"cofaxTools\",\n"
            "\"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\",\n"
            "\"init-param\": {\n"
            "\"templatePath\": \"toolstemplates/\",\n"
            "\"log\": 1,\n"
            "\"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\",\n"
            "\"logMaxSize\": \"\",\n"
            "\"dataLog\": 1,\n"
            "\"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\",\n"
            "\"dataLogMaxSize\": \"\",\n"
            "\"removePageCache\": \"/content/admin/remove?cache=pages&id=\",\n"
            "\"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\",\n"
            "\"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\",\n"
            "\"lookInContext\": 1,\n"
            "\"adminGroupID\": 4,\n"
            "\"betaServer\": true\n"
            "}\n"
            "}\n"
            "],\n"
            "\"servlet-mapping\": {\n"
            "\"cofaxCDS\": \"/\",\n"
            "\"cofaxEmail\": \"/cofaxutil/aemail/*\",\n"
            "\"cofaxAdmin\": \"/admin/*\",\n"
            "\"fileServlet\": \"/static/*\",\n"
            "\"cofaxTools\": \"/tools/*\"\n"
            "},\n"
            "\"taglib\": {\n"
            "\"taglib-uri\": \"cofax.tld\",\n"
            "\"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"\n"
            "}\n"
            "}\n"
            "}");
    printf("%d %ld\n", msg.c, (long int) msg.rtm);
    c_byte_bufferp buff = c_byte_buffer_create();
    pb_serialize_ng_msg(msg, buff);
    printf("%zu\n", buff->len);
    //NGmsg dmsgp = NGMSG__INIT;
    NGmsg dmsgp;
    pb_deserialize_ng_msg(buff, &dmsgp);//note the value-param
    printf("%d %ld %s\n", dmsgp.c, (long int)dmsgp.rtm, dmsgp.ip);
    printf("%s\n", dmsgp.d);
}

int test10() {
    NGmsg msg = NGMSG__INIT;
    msg.c = 123;
    msg.has_c = 1;
    msg.rtm = (int64_t)1432700228559;
    msg.has_rtm = 1;
    size_t len = ngmsg__get_packed_size(&msg);
    void * buff = malloc(len);
    printf("%d %ld\n", msg.c, msg.rtm);
    ngmsg__pack(&msg, buff);
    NGmsg * mp;
    mp = ngmsg__unpack(NULL, len, buff);
    printf("%d\n", mp->c);
    printf("%ld\n", (long int)mp->rtm);
}

void t_rd_logger(const rd_kafka_t *rkt, int level, const char *fac, const char *buf) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "%u.%03u RDKAFKA-%i-%s: %s: %s\n",
            (int)tv.tv_sec, (int)(tv.tv_usec / 1000),
            level, fac, rkt ? rd_kafka_name(rkt) : NULL, buf);
}

void t_msg_delivered(rd_kafka_t *rk,
                     void *payload, size_t len,
                     int error_code,
                     void *opaque, void *msg_opaque) {

    if (error_code)
        fprintf(stderr, "%% Message delivery failed: %s\n",
                rd_kafka_err2str(error_code));
    else
        fprintf(stderr, "%% Message delivered (%zd bytes)\n", len);
}

static void t_msg_delivered2(rd_kafka_t *rk,
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

/* test for librdkafka */
int test11() {
    // basic var
    char * brokers_list = "localhost:9092";
    char * topic = "console";
    int partition = RD_KAFKA_PARTITION_UA;
    char err[1024] = {'\0'};
    char tmp[128];

    // rd kafka statement
    rd_kafka_t * rkt = NULL;
    rd_kafka_conf_t * rkct = rd_kafka_conf_new();
    rd_kafka_conf_set_log_cb(rkct, t_rd_logger);
    rd_kafka_topic_t * rktt = NULL;
    rd_kafka_topic_conf_t * rktct = rd_kafka_topic_conf_new();

    // config
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(rkct, "internal.termination.signal", tmp, NULL, 0);
    rd_kafka_conf_set(rkct, "compression.codec", "snappy", err, sizeof(err));

    // producer
    char buff[2048];
    int sendcnt = 0;

    //  producer config
    rd_kafka_topic_conf_set(rktct, "produce.offset.report", "true", err, sizeof(err));
    rd_kafka_conf_set_dr_msg_cb(rkct, t_msg_delivered2);

    // create rd kafka handle
    rkt = rd_kafka_new(RD_KAFKA_PRODUCER, rkct, err, sizeof(err));
    if(!rkt) {
        fprintf(stderr, "%% Failed to create new producer: %s\n", err);
        exit(1);
    }
    rd_kafka_set_log_level(rkt, LOG_DEBUG);

    // add broker
    if(!rd_kafka_brokers_add(rkt, brokers_list)) {
        fprintf(stderr, "%% No valid brokers specified\n");
        exit(1);
    }

    // create topic
    rktt = rd_kafka_topic_new(rkt, topic, rktct);
    rktct = NULL;

    // send msg
    int i = 0;
    while (i++ <= 100) {
        char numstr[10];
        strcpy(buff, "hello kafka ");
        sprintf(numstr, "%d", i);
        strcat(buff, numstr);
        size_t len = strlen(buff);
        if(rd_kafka_produce(rktt, partition, RD_KAFKA_MSG_F_COPY, buff, len, NULL, 0, NULL) == -1) {
            sprintf(stderr, "%% Failed to produce to topic %s partition %i: %s\n", rd_kafka_topic_name(rktt), partition, rd_kafka_err2str(rd_kafka_last_error()));
            rd_kafka_poll(rkt, 0);
            continue;
        }
        sendcnt++;
        rd_kafka_poll(rkt, 0);
    }

    // destroy
    rd_kafka_poll(rkt, 0);
    while (rd_kafka_outq_len(rkt) > 0)
        rd_kafka_poll(rkt, 100);
    rd_kafka_topic_destroy(rktt);
    rd_kafka_destroy(rkt);
    if(rktct)
        rd_kafka_topic_conf_destroy(rktct);
    int run = 5;
    while (run-- && rd_kafka_wait_destroyed(1000) == -1)
        printf("Waiting for librdkafka to decommossion\n");
    if(run <= 0)
        rd_kafka_dump(stdout, rkt);

    return 0;
}

/* kafka producer handler */
typedef struct tu_rdkafka_producer_s {
    rd_kafka_t * rk;
    rd_kafka_conf_t * rk_conf;
    rd_kafka_topic_t * rk_topic[100]; //max 100 topics
    rd_kafka_topic_conf_t * rk_topic_conf;// only one topic conf
    char err_msg[1024];
    char broker_list[1024];
    char topics[100][64];
    int topic_len;
} tu_rdkafka_producer, *tu_rdkafka_producerp;

int test12() {
    tu_rdkafka_producerp p = (tu_rdkafka_producerp) malloc(sizeof(tu_rdkafka_producer));
    printf("%zu\n", sizeof(p->err_msg));
    return 0;
}

int test13() {
    u_rdkafka_producerp producer = u_rdkafka_producer_init();
    //customer config
    u_rdkafka_producer_connect(producer, "localhost:9092");
    u_rdkafka_add_topic(producer, "console");
    char * str = "kk monitor for librdkafka";
    u_rdkafka_send(producer, "console", str, strlen(str), NULL, 0);
    u_rdkafka_producer_close(producer);
    return 0;
}

int test14() {
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
    printf("%s\n", ip);
    return 0;
}

int test15() {
    char hname[128];
    struct hostent * hent;
    gethostname(hname, sizeof(hname));
    hent = gethostbyname(hname);
    if(hent->h_addr_list[1]) {
        char * ip = inet_ntoa(*(struct in_addr*) (hent->h_addr_list[0]));
        printf("%s\n", ip);
    }
    return 0;
}

int test16() {
    struct ifaddrs * ifaddr;
    getifaddrs(&ifaddr);
    struct ifaddrs * tmp = ifaddr;
    while (tmp) {
        if(tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *) tmp->ifa_addr;
            printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(ifaddr);
}

int test17() {
    char * str = malloc(sizeof(char) * 100);
    char * data = "get out of here !!";
    sprintf(str, "134 %s 321", data);
    printf("%s\n", str);
    free(str);// free str would not have effect on data, but free data will have effect on str, sprintf is not copy
    printf("%s\n", data);
    return 0;
}

/**
 * callback for statistics to send monitor
 * @param rk
 * @param json
 * @param json_len
 * @param opaque
 * @return free(0) or not free(1) json
 */
int stats_cb(rd_kafka_t *rk, char * json, size_t json_len, void * opaque) {
    send_producer_status_monitor(json);//send status to monitor
    return 0;
}

int test18() {
    u_rdkafka_producerp producer = u_rdkafka_producer_init();
    u_rdkafka_config_set(producer, "statistics.interval.ms", "5000");
    //set monitor callback
    rd_kafka_conf_set_stats_cb(producer->rk_conf, stats_cb);
    u_rdkafka_producer_connect(producer, "localhost:9092");
    u_rdkafka_add_topic(producer, "console");
    char str[128] = "kk monitor for librdkafka";
    for(int i = 0; i <= 20 - 1; i++) {
        sprintf(str, "kick clock monitor %d", i);
        u_rdkafka_send(producer, "console", str, strlen(str), NULL, 0);
        sleep(1);
    }
    u_rdkafka_producer_close(producer);
    return 0;
}

int main() {
    test18();
    return 0;
}