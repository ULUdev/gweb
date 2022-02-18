#include "linked_list.h"
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

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
	list->size++;
}

void *linked_list_pop(linked_list_t *list) {
	assert(list);
	if (list->size == 0) {
		return NULL;
	}
	assert(list->head);
	void *val = list->head->val;
	linked_list_node_t *old = list->head;
	list->head = list->head->next;
	free(old);
	list->size--;
	return val;
}

size_t linked_list_size(linked_list_t *list) {
	return list->size;
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
void linked_list_reverse(linked_list_t *list) {
	linked_list_t *reversed = linked_list_new();
	void *data = linked_list_pop(list);
	while (data) {
		linked_list_push(reversed, data);
		data = linked_list_pop(list);
	}
	linked_list_destroy(list);
	list = reversed;
}

linked_list_t *linked_list_dup(const linked_list_t *list) {
	linked_list_t *new = linked_list_new();
	linked_list_node_t *cur = list->head;
	while(cur) {
		linked_list_node_t *val = malloc(sizeof(linked_list_node_t));
		memcpy(val, cur, sizeof(linked_list_node_t));
		linked_list_push(new, val);
		cur = cur->next;
	}
	return new;
}
