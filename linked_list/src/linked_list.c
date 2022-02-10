#include "linked_list.h"
#include <stdlib.h>

struct LinkedListNode {
	void *val;
	struct LinkedListNode *next;
};

struct LinkedList {
	linked_list_node_t *head;
	size_t size;
};

linked_list_t *linked_list_new() {
	linked_list_t *list = malloc(sizeof(linked_list_t));
	list->head = NULL;
	list->size = 0;
	return list;
}

void linked_list_push(linked_list_t *list, void *val) {
	linked_list_node_t *new = malloc(sizeof(linked_list_node_t));
	new->val = val;
	new->next = list->head;
	list->head = new;
}

void *linked_list_pop(linked_list_t *list) {
	linked_list_node_t *current = list->head;
	list->head = current->next;
	void *val = current->val;
	free(current);
	return val;
}

void linked_list_destroy(linked_list_t *list) {
	linked_list_node_t *current = list->head;
	while (current != NULL) {
		linked_list_node_t *next = current->next;
		free(current);
		current = next;
	}
	free(list);
}
