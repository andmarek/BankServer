#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Bank.h"
#include "utils.h"
#include "queue.h"

void *event_loop();
void *handle_request_thread(void *arg);
uint8_t handle_balance_check(char **argv, request_t *, queue_node_t *);
uint8_t handle_trans(char **argv, request_t *);
void handle_exit(void);
void print_queue(queue_t *);

pthread_mutex_t q_lock;
pthread_cond_t io_cv;
pthread_cond_t worker_cv;

int main(int argc, char **argv)
{
    account_t *accounts;
    const char *responses;
    int i;
    int num_threads, num_accounts;
    uint8_t end; /* Determines if we exit */
    FILE *f;
    struct timeval time;
    queue_t *q;

    num_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    responses = argv[3];

    q = malloc(sizeof(queue_t));
    queue_init(q);

    pthread_mutex_init(&q_lock, NULL); /* Initialize lock */

    /* Create the producer thread */
    pthread_t io;
    pthread_create(&io, NULL, event_loop, q);

    pthread_cond_init(&worker_cv, NULL);

    pthread_t workers[num_threads];
    for (i = 0; i < num_threads; i++) { /* Create array of worker threads */
        pthread_create(&workers[i], NULL, handle_request_thread, q);
    }

    f = fopen(responses, "a");

    /* Initialize the accounts */
    accounts = malloc(num_accounts * sizeof(account_t));

    if (!initialize_accounts(num_accounts)) { /* We might need to lock some shit here */
        printf("Error: initialize accounts failed");
        return 1;
    }

    for (i = 0; i < num_accounts; i++) {
        pthread_mutex_init(&(accounts[i].lock), NULL); /* Each account has their own mutex! */
        accounts[i].value = 0;
    }

    pthread_join(io, NULL);

    /* Everything is good up until here */

    //event_loop(q);

    for (i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("All threads complete\n");

    return 0;
}

void *handle_request_thread(void *arg)
{

    queue_t *q = (queue_t *) arg;

    pthread_mutex_lock(&q_lock);

    while (is_empty(q)) {
        pthread_cond_wait(&worker_cv, &q_lock);
    }

    printf("Handling this yung request\n");

    queue_node_t *n;
    request_t *r; /* Keep this on the stack since we already allocated for the request in
                     event_loop */

    n = dequeue(q); /* Takes from top of the queue */

    printf("queue_size after dequeue: %d\n", q->size);

    r = n->datum;

    char **args = r->cmd;
    printf("args[0] %s\n", args[0]);

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

    if (is_empty(q)) {
        pthread_cond_broadcast(&io_cv);
    }

    pthread_mutex_unlock(&q_lock);

    return NULL;
}

void *event_loop(queue_t *q)
{
    pthread_mutex_lock(&q_lock);

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
//        printf("args[0] %s\n", args[0]);


//        print_queue(q);


        while (!is_empty(q)) {
            pthread_cond_wait(&io_cv, &q_lock);
        }

        enqueue(q, r); /* Put into queue q with r as the datum */
        id++;

        pthread_cond_broadcast(&worker_cv);

        pthread_mutex_unlock(&q_lock);

        //usleep(1); doesn't do anything I am sure
    } while (1);

}

/* On success, return 1 */
uint8_t handle_balance_check(char **argv, request_t *r, queue_node_t *n)
{
    printf("argv[0]: %s\n", argv[0]);
    printf("----------\n");
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

/* For debugging */
void print_queue(queue_t *q)
{
    printf("printing queue\n");
    char *contents;

    contents = malloc(sizeof(char) * 1000);

    queue_node_t *cur;
    cur = q->head;

    while (cur->next != NULL) {
        strcat(contents, ((request_t *) cur->datum)->cmd[0]);
        if (((request_t *) cur->datum)->cmd[1]) {
            strcat(contents, ((request_t *) cur->datum)->cmd[1]);
        }
        cur = cur->next;
    }

    printf("Contents: %s \n", contents);
}
