#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "Bank.h"
#include "utils.h"
#include "queue.h"

void *event_loop();
void *handle_request_thread(void *arg);
uint8_t handle_balance_check(char **argv, queue_node_t *);
uint8_t handle_trans(char **argv, request_t *);
void handle_exit(void);
void print_queue(queue_t *);

FILE *f;
struct timeval t;
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

/*** TESTING 123 */
    write_account(1, 5);

/***/

    pthread_join(io, NULL);

    for (i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("All threads complete\n");

    fclose(f);

    return 0;
}

void *handle_request_thread(void *arg)
{
    queue_t *q = (queue_t *) arg;

    pthread_mutex_lock(&q_lock);

    while (is_empty(q)) {
        pthread_cond_wait(&io_cv, &q_lock);
    }

    printf("---Handling request---\n");

    queue_node_t *n;
    request_t *r;

    n = dequeue(q);

    r = n->datum;

    char **args = r->cmd;

    if (strncmp(args[0], "CHECK", 5 ) == 0) {
        printf("---Handling balance check---\n");
        handle_balance_check(args, n);
    } else if (strncmp(args[0], "TRANS", 5 ) == 0) {
        printf("Handling transaction\n");
        handle_trans(args, r);
    } else if (strncmp(args[0], "END", 3 ) == 0) {
        printf("Handling exit\n");
        handle_exit();
    } else {
        printf("Invalid input: %s\n", r->cmd[0]);
    }

    pthread_mutex_unlock(&q_lock);

    return NULL;
}

void *event_loop(queue_t *q)
{
    pthread_mutex_lock(&q_lock);

    char *line;
    char **args;
    int id = 1;

    do {

        printf("> ");

        fflush(stdout);

        line = read_line();
        args = split_line(line);

        queue_node_t *n;

        gettimeofday(&t, NULL);

        request_t *r = malloc(sizeof(request_t));
        r->cmd = args; r->request_id = id;
        r->starttime = t;

        enqueue(q, r);

        id++;

        pthread_cond_broadcast(&io_cv);

        pthread_mutex_unlock(&q_lock);


    } while (1);

}

uint8_t handle_balance_check(char **argv, queue_node_t *n)
{

    int balance;
    int acc_id;
    int req_id;
    request_t *r = (request_t *)(n->datum);

    acc_id = atoi(argv[1]);

    balance = read_account(acc_id);

    req_id = ((request_t *) n->datum)->request_id;

    printf("< ID %d\n", req_id);

    /* we make a string with the following
        - request id
        - balance of the account
        - time stuff
     */


    char *message = malloc(sizeof(char) * 50);

    gettimeofday(&t, NULL);

    r->endtime = t;

    //printf("TIME %ld.%06.ld\n", t.tv_sec, t.tv_usec);
    fprintf(f, "dogs\n");
    fprintf(f, "%d BAL %d TIME %ld.%06.ld %ld.%06.ld \n", req_id, balance, r->starttime.tv_sec,
            r->starttime.tv_usec, r->endtime.tv_sec, r->endtime.tv_usec);
    printf("%s\n", message);

    fflush(f);

    //fwrite(message);

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
