#include <stdint.h>

#define NONCELEN sizeof(uint8_t)*20

#ifndef LIST_H
#define LIST_H

typedef struct node {
    uint8_t * val;
    struct node* next;
    struct node* prev;
} node_t;

node_t * l_create(const uint8_t * val);

void l_append(node_t * head, const uint8_t * val);

void l_prepend(node_t ** head, const uint8_t * val);

uint8_t * l_pop(node_t ** head);

uint8_t * l_truncate(node_t * head);

int l_len(node_t * head);

uint8_t * l_extract(node_t ** head, int n);

void l_delete(node_t * head);

#endif
