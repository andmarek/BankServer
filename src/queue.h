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
queue_node_t *enqueue(queue_t *q, void *d);
queue_node_t *get_front(queue_t *q);
queue_node_t *dequeue(queue_t *q);
uint8_t is_empty(queue_t *q);
//void print_queue(queue_t *q);

#endif
