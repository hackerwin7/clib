//
// Created by fff on 3/14/16.
// gc for the pointer malloc by ring buffer

#include "stdio.h"
#include "stdlib.h"

#include "gc_ring_buffer.h"

#define RL 3000

typedef struct ring_elem_s {
    void *p;//the value of the pointer that point a segment of memory
    struct ring_elem_s * next;
}ring_elem, * ring_elemp;

ring_elemp header_pt_ring_gc = NULL;
ring_elemp rear_pt_ring_gc = NULL;
ring_elemp cur_pt_ring_gc = NULL;

/**
 * init the ring buffer, rear elem point the head elem
 * @return status
 */
int init_ring_buffer() {
    header_pt_ring_gc = NULL;
    rear_pt_ring_gc = NULL;
    cur_pt_ring_gc = NULL;
    for(int i = 0; i <= RL - 1; i++) {
        ring_elemp pt = (ring_elemp) malloc(sizeof(ring_elem));
        pt->p = NULL;
        pt->next = NULL;
        rear_pt_ring_gc = pt;// point the temp end link list
        if(!i)//i == 0
            header_pt_ring_gc = pt;
        else {
            rear_pt_ring_gc->next = pt;//link the elem
            rear_pt_ring_gc = pt;//rear pointer take a advance
        }
    }
    rear_pt_ring_gc->next = header_pt_ring_gc;//link to ring
    cur_pt_ring_gc = header_pt_ring_gc;
}

/**
 * put the memory pointer into the ring buffer
 * if current ring buffer is full, free the old and put this new one, then cur pointer take a advance
 * @param elem
 * @return status
 */
int put_ring_gc(void *elem) {
    if(!cur_pt_ring_gc || !header_pt_ring_gc || !rear_pt_ring_gc)
        init_ring_buffer();
    if(cur_pt_ring_gc->p) { //free the mem
        free(cur_pt_ring_gc->p);
        cur_pt_ring_gc->p = NULL;
    }
    cur_pt_ring_gc->p = elem;
    cur_pt_ring_gc = cur_pt_ring_gc->next;//take a advance
}

/**
 * malloc the memory for the ring buffer
 * @param len
 * @return void * pointer
 */
void * malloc_ring_gc(size_t len) {
    void * p = NULL;
    p = malloc(len);
    put_ring_gc(p);//put into ring buffer
    return p;
}