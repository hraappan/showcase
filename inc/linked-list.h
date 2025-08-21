#pragma once

#include <stdlib.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

LinkedList *ll_init_list(void);
void ll_destroy_list(LinkedList *);
void *ll_append_data_to_list(LinkedList *, void *, size_t);
void *ll_pop_data_from_list(LinkedList *);