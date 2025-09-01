/******************************************************************************
 *  hashmap.c
 *
 *  A simple hash map implementation in C with open addressing.
 *
 *  Author: Hannu Raappana
 *  Created: 2025-09-01
 *
 *  Description:
 *  This module provides basic hash map operations, including:
 *   - HM_init(): initialize a new hash map
 *   - HM_destroy(): free all entries and memory
 *   - HM_add_value(): insert or update a key-value pair
 *   - HM_get_value(): retrieve the value for a given key
 *   - HM_remove_value(): remove a key-value pair
 *
 *  Implementation details:
 *   - Uses integer keys (int) and generic pointers (void*) for values
 *   - Collision resolution with linear probing
 *   - Does not resize automatically (capacity fixed at initialization)
 *
 *  License: MIT License
 *
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
 *****************************************************************************/

#include "hashmap.h"
#include "logging.h"

#include <stdlib.h>

HashMap *HM_init(int capacity)
{
    HashMap *hm = malloc(sizeof(HashMap));
    if (hm == NULL) return NULL;

    hm->entries = calloc(capacity, sizeof(HashEntry));
    if (hm->entries == NULL) {
        free(hm);
        ERROR_PRINT("Cannot malloc memory for HashMap entries size %i", capacity * sizeof(HashEntry));
        return NULL;
    }
    hm->capacity = capacity;
    hm->size = 0;

    DEBUG_PRINT("HashMap is now initialized with size of %i", capacity * sizeof(HashEntry));
    return hm;
}

void *HM_add_value(HashMap *hm, int key, void *value) {
    if (hm->size >= hm->capacity) {
        ERROR_PRINT("HashMap is full");
        return NULL;
    }

    int idx = key % hm->capacity;
    for (int i = 0; i < hm->capacity; i++) {
        int try = (idx + i) % hm->capacity;
        HashEntry *entry = &hm->entries[try];

        if (!entry->used) {
            entry->key = key;
            entry->value = value;
            entry->used = 1;
            hm->size++;
            return value;
        }
        else if (entry->key == key) {
            entry->value = value;
            return value;
        }
    }

    ERROR_PRINT("HashMap insert failed (should not happen)");
    return NULL;
}

void *HM_get_value(HashMap *hm, int key)
{
    if (hm->size == 0) {
        DEBUG_PRINT("HashMap is empty");
        return NULL;
    }

    int idx = key % hm->capacity;
    for (int i = 0; i < hm->capacity; i++) {
        int get = (idx + i) % hm->capacity;
        HashEntry *entry = &hm->entries[get];

        if (!entry->used) {
            break;
        }
        if (entry->key == key) {
            return entry->value;
        }
    }

    DEBUG_PRINT("There is no key %i value pair", key);
    return NULL;
}

void HM_free(HashMap **hm)
{
    if (hm == NULL || *hm == NULL) return;

    if ((*hm)->entries != NULL)
        free((*hm)->entries);
    free((*hm));
}
