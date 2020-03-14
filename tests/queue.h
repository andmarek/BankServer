#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

struct queue_node;
typedef struct queue_node queue_node_t;

/*typedef struct queue_node {
    struct queue_node *next;
    int datum;
} queue_node_t;*/

typedef struct queue {
    queue_node_t *head;
    uint32_t size;
} queue_t;

void queue_init(queue_t *q);
queue_node_t *add(queue_t *q, void *d);

#endif
