#include <list.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

node_t * l_create(const uint8_t val[]) {
    uint8_t * data = malloc(NONCELEN);
    memcpy(data, val, NONCELEN);

  	node_t * head = NULL;
  	head = (node_t *) malloc(sizeof(node_t));

  	head->val = data;
  	head->next = NULL;
  	head->prev = NULL;
  	return head;
}

void l_append(node_t * head, const uint8_t val[]) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    uint8_t * data = malloc(NONCELEN);
    memcpy(data, val, NONCELEN);

    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->val = data;
    current->next->next = NULL;
    current->next->prev = current;
}

void l_prepend(node_t ** head, const uint8_t val[]) {
    node_t * new_node;
    new_node = (node_t *) malloc(sizeof(node_t));

    uint8_t * data = malloc(NONCELEN);
    memcpy(data, val, NONCELEN);

    new_node->val = data;
    new_node->next = *head;
    new_node->prev = NULL;
    (*head)->prev = new_node;
    *head = new_node;
}

uint8_t * l_truncate(node_t * head) {
    uint8_t * retval = NULL;
    if (head->next == NULL) {
        retval = head->val;
        free(head);
        return retval;
    }

    node_t * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    retval = current->next->val;
    free(current->next);
    current->next = NULL;
    return retval;
}

uint8_t * l_pop(node_t ** head) {
    uint8_t * retval = NULL;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return NULL;
    }

    next_node = (*head)->next;
    next_node->prev = NULL;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}

uint8_t * l_extract(node_t ** head, int n) {
    int i = 0;
    uint8_t * retval = NULL;
    node_t * current = *head;
    node_t * temp_node = NULL;

    if (n == 0) {
        return l_pop(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return NULL;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->val;
    current->next = temp_node->next;
    temp_node->next->prev = current;
    free(temp_node);

    return retval;
}

int l_len(node_t * head) {
    if (head == NULL) {
        return 0;
    }

    int res = 1;
    node_t * curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
        res++;
    }
    return res;
}

void l_delete(node_t * head) {
    if (head == NULL) {
	    return;
    }

    node_t * next_node = head->next;
    free(head);
    l_delete(next_node);
}
