#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
/*
struct queue_node;
typedef struct queue_node queue_node_t;*/

typedef struct queue_node {
    struct queue_node *next;
    void *datum;
} queue_node_t;

typedef struct queue {
    struct queue_node *head;
    struct queue_node *rear;
    uint32_t size;
} queue_t;

void queue_init(queue_t *q);
queue_node_t *add(queue_t *q, void *d);

#endif
