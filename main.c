#include <stdio.h>
#include "string.h"

#include "common/common.h"
#include "zk-lib/zk_util.h"
#include "gzip-lib/gzip_util.h"
#include "base64-lib/base64_util.h"

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

int main() {
    test3();
    return 0;
}