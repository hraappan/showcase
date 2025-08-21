/******************************************************************************
 *  Ring Buffer Implementation
 *  
 *  File: ring-buffer.c
 *  Description: Implements a fixed-size circular buffer (ring buffer) for byte
 *               storage with overwrite support. Supports reading and writing
 *               with wrap-around handling, as well as checking free space and
 *               readable data.
 *
 *  Author: Hannu Raappana
 *  Created: 2025-08-19
 *  License: MIT License
 *
 *  Notes:
 *  - Overwrites old data when the buffer is full.
 *  - Wrap-around handled automatically.
 *  - Thread-safety is NOT implemented; use appropriate locking if needed.
 *
 *  MIT License
 *  -----------
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 ******************************************************************************/

#include "ring-buffer.h"
#include "logging.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t rbuf_readable(RingBuffer *rb)
{
    size_t total = sizeof(rb->data);
    if (rb->head >= rb->tail)
        return rb->head - rb->tail;
    else
        return total - rb->tail + rb->head;
}

size_t rbuf_free_space(RingBuffer *rb)
{
    size_t total = sizeof(rb->data);
    return total - rbuf_readable(rb);
}

void rbuf_store_data(RingBuffer *rb, uint8_t *src, size_t count)
{
    size_t total = sizeof(rb->data);

    if (count >= total) {
        src += count - total;
        count = total;
        rb->tail = 0;
        rb->head = 0;
    }

    size_t free = rbuf_free_space(rb);
    if (count > free) {
        size_t over = count - free;
        rb->tail = (rb->tail + over) % total; 
    }

    size_t first = total - rb->head;
    if (count > first) {
        memcpy(rb->data + rb->head, src, first);
        memcpy(rb->data, src + first, count - first);
    } else {
        memcpy(rb->data + rb->head, src, count);
    }

    rb->head = (rb->head + count) % total;
    DEBUG_PRINT("store: tail %zu, head %zu, count %zu", rb->tail, rb->head, count);
}

size_t rbuf_pop_data(RingBuffer *rb, uint8_t *dest, size_t count)
{
    size_t total = sizeof(rb->data);
    size_t read = rbuf_readable(rb);
    if (read == 0) {
        DEBUG_PRINT("Buffer empty, tail %zu, head %zu", rb->tail, rb->head);
        return 0;
    }
    if (count > read)
        count = read;

    size_t first = total - rb->tail;
    if (count > first) {
        memcpy(dest, rb->data + rb->tail, first);
        memcpy(dest + first, rb->data, count - first);
    } else {
        memcpy(dest, rb->data + rb->tail, count);
    }

    rb->tail = (rb->tail + count) % total;
    DEBUG_PRINT("pop: tail %zu, head %zu, count %zu", rb->tail, rb->head, count);

    return count;
}

size_t rbuf_get_buffer_size(RingBuffer *rb)
{
    return sizeof(rb->data);
}

size_t rbuf_get_free_space(RingBuffer *rb)
{
    return rbuf_free_space(rb);
}

RingBuffer *rbuf_init_buffer(void)
{
    RingBuffer *buff = malloc(sizeof(RingBuffer));
    if (buff) {
        buff->head = 0;
        buff->tail = 0;
    }
    return buff;
}

void rbuf_free_buffer(RingBuffer *rb)
{
    if (rb == NULL) return;
    free(rb);
}
