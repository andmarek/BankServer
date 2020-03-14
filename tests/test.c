#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main()
{
    queue_t *q;
    queue_init(q);

    int x = 5;

    if (!insert_node(q, &x)) {
        perror("dogs\n");
    }
    //printf("%d\n", q->head);

    return 0;
}
