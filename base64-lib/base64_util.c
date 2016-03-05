//
// Created by fff on 3/5/16.
//

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "base64_util.h"

#define MAX 0xFF

/* global base64 original code table */
static char sta_code_table[64] = {
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',// 0 ~ 9
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',// 10 ~ 19
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',// 20 ~ 29
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',// 30 ~ 39
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',// 40 ~ 49
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',// 50 ~ 59
        '8', '9', '+', '/' // 60 ~ 63
};

/* static function header */
static char find_pos(char ch);

/**
 * find the index of the ch in the code table
 * @param ch
 */
char find_pos(char ch) {
    char * pt = (char *) strrchr(sta_code_table, ch); // find the last position in the code table
    return (char)(pt - sta_code_table);
}

/**
 * encode to base64
 * @param src
 * @param des
 */
int base64_encode(char * src, char * des) {
    int prepare = 0;
    int ret_len = 0;
    int temp = 0;
    int tmp = 0;
    char changed[4];
    int des_p = 0;
    int slen = (int) strlen(src);
    ret_len = slen / 3; //every 3 bytes is a unit
    temp = slen % 3;
    if(temp > 0)
        ret_len += 1;
    ret_len = ret_len * 4 + 1;
    memset(des, '\0', ret_len);
    while (tmp < slen) {
        temp = 0;
        prepare = 0;
        memset(changed, '\0', 4);
        while (temp < 3) {
            if(temp >= slen)
                break;
            prepare = ((prepare << 8) | (src[tmp] & 0xFF));
            tmp++;
            temp++;
        }
        prepare = (prepare << ((3 - temp) * 8));
        for(int i = 0; i < 4; i++) {
            if(temp < i)
                changed[i] = 0x40;
            else
                changed[i] = (char) ((prepare >> ((3 - i) * 6)) & 0x3F);
            des[des_p++] = sta_code_table[(int)changed[i]];
        }
    }
    des[des_p] = '\0';//rear end
    return 0;
}

/**
 * decode to string
 * @param src
 * @param des
 */
int base64_decode(char * src, char * des) {
    int slen = (int) strlen(src);
    int ret_len = (slen / 4) * 3;
    int equal_count = 0;
    int tmp = 0;
    int temp = 0;
    char need[3];
    int prepare = 0;
    for(int i = slen -1; i>= slen - 3; i--)
        if(src[i] == '=')
            equal_count += 1;
    switch (equal_count) {
        case 0:
            ret_len += 4;
            break;
        case 1:
            ret_len += 4;
            break;
        case 2:
            ret_len += 3;
            break;
        case 3:
            ret_len += 2;
            break;
    }
    memset(des, '\0', ret_len);
    int wlen = slen - equal_count;
    int des_pt = 0;
    while (tmp < wlen) {
        temp = 0;
        prepare = 0;
        memset(need, '\0', 3);
        while (temp < 4) {
            if(tmp >= wlen)
                break;
            prepare = (prepare << 6) | (find_pos(src[tmp]));
            temp++;
            tmp++;
        }
        prepare = prepare << ((4 - temp) * 6);
        for(int i = 0; i < 3; i++) {
            if(temp == i)
                break;
            des[des_pt++] = (char) ((prepare >> ((2 - i) * 8)) & 0xFF);
        }
    }
    des[des_pt] = '\0';//rear end
    return 0;
}