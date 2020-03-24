#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"

/* Our constructor */
void queue_init(queue_t *q)
{
    q->head = NULL;
    q->rear = NULL;

    q->size = 0;
}

/* Insert a new node into the queue */
queue_node_t *enqueue(queue_t *q, void *v)
{

    queue_node_t *n;
    n =  malloc(sizeof(queue_node_t));

    n->datum = v;
    n->next = NULL;

    /* If our list is empty */
    if (!q->head) {
        q->head = n;
        q->rear = n;
    } else {
        q->rear->next = n;
        q->rear = q->rear->next;
    }

    q->size++;

    return n;
}

queue_node_t *dequeue(queue_t *q)
{
    queue_node_t *r;
    r = q->head;
    q->head = q->head->next;
    q->size--;
    /* Do we need to free ? */
    return r;
}

queue_node_t *get_front(queue_t *q)
{
    return q->head;
}

uint8_t is_empty(queue_t *q)
{
    if (q->size <= 0) {
        return 1;
    }

    return 0;
}
/*
void print_queue(queue_t *q)
{
    printf("printing queue\n");
    char *contents;

    contents = malloc(sizeof(char) * 1000);

    queue_node_t *cur;
    cur = q->head;


    while (cur->next != NULL) {
        strcat(contents, (cur->datum));
        cur = cur->next;
    }

    printf("Contents: %s \n", contents);
}
*/
