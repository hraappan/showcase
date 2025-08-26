/******************************************************************************
 *  memory-pool.c
 *
 *  A simple fixed-block memory pool implementation in C with bitmap tracking.
 *  Supports pool initialization, allocation, deallocation, and destruction.
 *  Provides double-free protection using a bitmap.
 *
 *  Author: Hannu Raappana
 *  Created: 2025-08-19
 *
 *  License: MIT License
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 ******************************************************************************/

#include <stdint.h>
#include <math.h>

#include "memory-pool.h"
#include "logging.h"

MemoryPool *pool_init(size_t bsize, size_t bcount)
{
    MemoryPool *mp = (MemoryPool *) malloc(sizeof(MemoryPool));
    if (mp == NULL) {
        ERROR_PRINT("Cannot allocate memory, block-size %li, block-count %li", bsize, bcount);
        return NULL;
    }
    mp->block_size = bsize;
    mp->total_blocks = bcount;
    mp->pool = malloc(bsize * bcount);
    mp->free_count = bcount;
    mp->flist = malloc(sizeof(void *) * bcount);
    mp->block_map = calloc(1, (size_t) ceil((double) bcount / 8.0));

    for (size_t i=0; i<bcount; i++) {
        mp->flist[i] = ((char *) mp->pool + (i * bsize));
    }

    DEBUG_PRINT("Memory pool initialized. Block- size %zu, block count %zu. \
        Start address: %p \
        End address: %p", \
        bsize, bcount, mp->pool, (char *) mp->pool + bcount * bsize);
    return mp;
}

void *pool_malloc(MemoryPool *mp)
{
    if (mp == NULL) {
        ERROR_PRINT("Uninitialized memory pool");
        return NULL;
    } else if (!mp->free_count) {
        ERROR_PRINT("Out of Memory");
        return NULL;
    }

    void *block = NULL;
    block = mp->flist[--mp->free_count];
    DEBUG_PRINT("Memory allocated from address %p, block-size %zu, \
        free blocks left %zu", block, mp->block_size, mp->free_count);

    WRITE_BITMAP(mp->block_map, mp->free_count);
    return block;
}

void pool_free(MemoryPool *mp, void *block)
{
    if (mp == NULL) {
        ERROR_PRINT("Uninitialized memory pool");
        return;
    }

    uintptr_t pool_start = (uintptr_t) mp->pool;
    uintptr_t pool_end   = pool_start + mp->block_size * mp->total_blocks;
    uintptr_t blk_addr   = (uintptr_t) block;

    if (blk_addr < pool_start || blk_addr >= pool_end) {
        ERROR_PRINT("Block address %p outside of memory pool", block);
        return;
    }

    size_t index = (blk_addr - pool_start) / mp->block_size;
    if (READ_BITMAP(mp->block_map, index) == 0) {
        ERROR_PRINT("Double free detected for block %p (index %zu)", block, index);
        return;
    }

    CLEAR_BITMAP(mp->block_map, index);

    if (mp->free_count < mp->total_blocks) {
        mp->flist[mp->free_count++] = block;
    } else {
        ERROR_PRINT("Trying to free unallocated memory block %p.", block);
        return;
    }

    DEBUG_PRINT("Memory freed block %p (index %zu)", block, index);
}

void pool_destroy(MemoryPool *mp)
{
    if (mp == NULL) return;
    free(mp->pool);
    free(mp->flist);
    free(mp->block_map);
    free(mp);

    DEBUG_PRINT("Memory pool destroyed");
}
