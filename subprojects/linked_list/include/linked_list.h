#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__
#include <stddef.h>
typedef struct LinkedListNode linked_list_node_t;
typedef struct LinkedList linked_list_t;

linked_list_t *linked_list_new();
void linked_list_push(linked_list_t *list, void *val);
void *linked_list_pop(linked_list_t *list);
size_t linked_list_size(linked_list_t *list);
void linked_list_reverse(linked_list_t *list);

void linked_list_destroy(linked_list_t *list);
linked_list_t *linked_list_dup(const linked_list_t *list);
#endif
