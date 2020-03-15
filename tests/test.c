#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main()
{
    queue_t *q = malloc(sizeof(queue_t));
    queue_init(q);

    int x = 5;
    int y = 6;
    int z = 7;

    enqueue(q, &x);
    enqueue(q, &y);
    enqueue(q, &z);

    printf("Adding node: %d\n", *((int *) (q->head->datum)));
    printf("Adding node: %d\n", *((int *) (q->head->next->datum)));
    printf("Adding node: %d\n", *((int *) (q->rear->datum)));

    printf("Dequeue node: %d\n", *((int *) (dequeue(q)->datum)));

    queue_node_t *cur = q->head;

    int count = 1;

    while (cur != NULL) {
        printf("Printing node %d: %d\n", count, *((int *)(cur->datum)));
        cur = cur->next;
    }

    printf("Get front test: %d\n", *((int *) get_front(q)->datum));
    return 0;
}

