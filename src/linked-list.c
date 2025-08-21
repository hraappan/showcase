/*
 * linked-list.c
 * 
 * A simple, dynamically allocated linked list implementation in C.
 *
 * Author: Hannu Raappana
 * Created: 2025-08-20
 *
 * Description:
 * This module provides basic linked list operations, including:
 *  - ll_init_list(): initialize a new empty linked list
 *  - ll_destroy_list(): free all nodes and data in the list
 *  - ll_append_list(): append a new node with data to the end of the list
 *  - ll_pop_data_from_list(): remove the first node and return its data
 *
 * License: MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>
#include "linked-list.h"
#include "logging.h"

LinkedList *ll_init_list(void)
{
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    DEBUG_PRINT("LinkedList is now initialized");
    return list;
}

void ll_destroy_list(LinkedList *list)
{
    if (!list) return;
    Node *cur = list->head;
    while (cur) {
        Node *tmp = cur->next;
        free(cur->data);
        free(cur);
        cur = tmp;
    }
    free(list);
}

void *ll_append_list(LinkedList *list, void *data, size_t data_size)
{
    if (!list) return NULL;
    DEBUG_PRINT("LinkedList append data size %zu", data_size);

    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    node->data = malloc(data_size);
    if (!node->data) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, data_size);
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;

    return node->data;
}

void *ll_pop_data_from_list(LinkedList *list)
{
    if (!list || !list->head) return NULL;

    Node *node = list->head;
    void *data = node->data;

    list->head = node->next;
    if (!list->head) {
        list->tail = NULL;
    }
    free(node);

    return data;
}
