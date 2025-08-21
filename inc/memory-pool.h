#pragma once

#include <stdlib.h>

#define WRITE_BITMAP(bmap, i) \
    ((bmap)[(i)/8] |=  (1 << ((i)%8)))
#define CLEAR_BITMAP(bmap, i) \
    ((bmap)[(i)/8] = (unsigned char)((bmap)[(i)/8] & ~(1U << ((i)%8))))
#define READ_BITMAP(bmap, i) \
    (((bmap)[(i)/8] >> ((i)%8)) & 1)

typedef struct MEM_P {
    void *pool;
    size_t block_size;
    size_t total_blocks;
    void **flist;
    size_t free_count;
    unsigned char *block_map;
} MemoryPool;

MemoryPool *pool_init(size_t block_size, size_t block_count);
void *pool_malloc(MemoryPool *);
void pool_free(MemoryPool *, void *block);
void pool_destroy(MemoryPool *);