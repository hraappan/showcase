#pragma once

typedef struct ENTRY_T {
    int key;
    void *value;
    int used;
} HashEntry;

typedef struct HASH_T {
    HashEntry *entries;
    int capacity;
    int size;
} HashMap;

HashMap *HM_init(int capacity);
void *HM_add_value(HashMap *hm, int key, void *value);
void *HM_get_value(HashMap *hm, int key);
void HM_free(HashMap **);