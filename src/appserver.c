#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"
#include "io.h"
#include "request.h"
#include "transaction.h"
#include "queue.h"

void event_loop();
void *handle_request_thread(void *arg);
uint8_t handle_balance_check(char **argv, request_t *, queue_node_t *);
uint8_t handle_trans(char **argv, request_t *);
void handle_exit(void);

pthread_mutex_t q_lock;

int main(int argc, char **argv)
{
    const char *responses;
    int i;
    int num_threads, num_accounts;
    uint8_t end; /* Determines if we exit */
    FILE *f;
    struct timeval time;
    queue_t *q;

    //queue_init(q);

    pthread_mutex_init(&q_lock, NULL); /* Initialize lock */

    num_threads = atoi(argv[1]);
    pthread_t workers[num_threads];
    for (i = 0; i < num_threads; i++) { /* Create array of worker threads */
        pthread_create(&workers[i], NULL, handle_request_thread, q);
    }

    num_accounts = atoi(argv[2]);
    responses = argv[3];

    f = fopen(responses, "a");

    /* Initialize the accounts */
    if (initialize_accounts(num_accounts) != 1) { /* We might need to lock some shit here */
        printf("Error: initialize accounts failed");
        return 1;
    }

    printf("%d accounts created.\n", num_accounts);

    /* Let's create the threads here */
    printf("%d threads created. \n", num_threads);

    event_loop();

    for (i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("All threads complete\n");

    return 0;
}

void *handle_request_thread(void *arg)
{
    queue_t *q = (queue_t *) arg;

    queue_node_t *n;
    request_t *r; /* Keep this on the stack since we already allocated for the request in
                     event_loop */

    while (!is_empty(q)) {
        n = dequeue(q); /* Takes from top of the queue */
        r = n->datum;
        char **args = r->cmd;
        if (strncmp(args[0], "CHECK", 5 ) == 0) {
            // handle balance check
            printf("Handling balance check\n");
            handle_balance_check(args, r, n);
        } else if (strncmp(args[0], "TRANS", 5 ) == 0) {
            // handle transaction
            printf("Handling transaction\n");
            handle_trans(args, r);
        } else if (strncmp(args[0], "END", 3 ) == 0) {
            // handle exit
            printf("Handling exit\n");
            handle_exit();
        } else {
            printf("Invalid input: %s\n", r->cmd[0]);
        }
    }
}

void event_loop(queue_t *q)
{
//    pthread_mutex_lock(&q_lock);

    char *line;
    char **args;
    int id = 0;

    do {
        printf("> ");

        line = read_line();
        args = split_line(line);

        queue_node_t *n;

        request_t *r = malloc(sizeof(request_t));
        r->cmd = args;
        enqueue(q, r);
        id++;
    } while (1);

}

/* On success, return 1 */
uint8_t handle_balance_check(char **argv, request_t *r, queue_node_t *n)
{
    printf("This is a test\n");
    //n->datum = argv;
    printf("argv[0]: %s\n", argv[1]);
    return 0;
}

/* On success, return 1 */
uint8_t handle_trans(char **argv, request_t *r)
{
    return 0;
}

void handle_exit(void)
{
    printf("Exiting. . .\n");
}
