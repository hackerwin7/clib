//
// Created by fff on 3/14/16.
//

#ifndef CLIB_GC_RING_BUFFER_H
#define CLIB_GC_RING_BUFFER_H

#endif //CLIB_GC_RING_BUFFER_H

/**
 * malloc the memory for the ring buffer
 * @param len
 * @return void * pointer
 */
void * malloc_ring_gc(size_t len);

/* put the mem pointer into ring buffer */
int put_ring_gc(void *elem);