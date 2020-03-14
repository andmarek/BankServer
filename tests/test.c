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

    if (!add(q, &x)) {
        perror("dogs\n");
    }
    add(q, &z);
    if (!add(q, &y)) {
        perror("dogs\n");
    }
    printf("%d\n", *((int *) (q->head->datum)));
    printf("%d\n", *((int *) (q->head->next->datum)));
    printf("%d\n", *((int *) (q->rear->datum)));

    return 0;
}

