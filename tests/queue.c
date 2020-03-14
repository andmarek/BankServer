#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

struct queue_node {
    queue_node_t *next;
    void *datum;
};

/* Our constructor */
void queue_init(queue_t *q)
{
    q->head = NULL;
    //q->rear = NULL;

    q->size = 0;
}

/* Insert a new node into the queue */
queue_node_t *add(queue_t *q, void *v)
{
    printf("Insert node entered.\n");

    queue_node_t *n;
    n =  malloc(sizeof(queue_node_t));

    n->datum = v;
    n->next = NULL;

    /* If our list is empty */
    if (q->head) {
        printf("Head was empty\n");
        q->head = n;
    }

    q->size++;

    return n;
}
