#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main()
{
    queue_t *q = malloc(sizeof(queue_t));
    queue_init(q);

    int x = 5;

    if (!add(q, &x)) {
        perror("dogs\n");
    }
    printf("%d\n", (int *) q->head->datum);

    return 0;
}

