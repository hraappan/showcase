#pragma once

#include <stdint.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

typedef struct BUFF_T {
    uint8_t data[BUFFER_SIZE];
    size_t head;
    size_t tail;
} RingBuffer;

void rbuf_store_data(RingBuffer*, uint8_t *src, size_t count);
size_t rbuf_pop_data(RingBuffer*, uint8_t *dest, size_t count);
size_t rbuf_get_buffer_size(RingBuffer *);
size_t rbuf_get_free_space(RingBuffer *);
RingBuffer *rbuf_init_buffer(void);
void rbuf_free_buffer(RingBuffer *);
