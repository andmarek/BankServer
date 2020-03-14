#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

struct queue_node {
    queue_node_t *next;
    queue_node_t *prev;
    void *datum;
};

void queue_init(queue_t *q)
{
    q = malloc(1*sizeof(queue_t));
    q->head = NULL;
    q->rear = NULL;
    q->size = 0;
}

queue_node_t *insert_node(queue_t *q, void *d)
{
    queue_node_t *n = malloc(sizeof(queue_node_t));

    q->rear->next = n;
    n->datum = d;
    n->prev = q->rear;
    q->rear = n;
    q->size++;

    return n;
}
