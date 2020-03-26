#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "Bank.h"
#include "utils.h"
#include "queue.h"

static void *    event_loop();
static void *    handle_request_thread(void *);
static uint8_t   handle_balance_check(char **argv, queue_node_t *);
static uint8_t   handle_trans(char **argv, queue_node_t *);
static int       process_trans(request_t *, int);
static uint8_t   handle_exit(void);
static void      print_queue(queue_t *);

FILE *f;
struct timeval t;
account_t *accounts;
uint8_t end; /* Determines if we exit */
pthread_mutex_t q_lock;
pthread_cond_t io_cv;
pthread_cond_t worker_cv;

int
main(int argc, char **argv)
{
        const char *responses;
        int i;
        int num_threads, num_accounts;
        queue_t *q;
        pthread_t io;
        pthread_t workers[num_threads];

        end = 0;

        num_threads = atoi(argv[1]);
        num_accounts = atoi(argv[2]);
        responses = argv[3];

        q = malloc(sizeof(queue_t));
        queue_init(q);

        pthread_mutex_init(&q_lock, NULL);

        pthread_create(&io, NULL, event_loop, q);

        pthread_cond_init(&worker_cv, NULL);

        for (i = 0; i < num_threads; i++) {
                pthread_create(&workers[i], NULL, handle_request_thread, q);
        }

        f = fopen(responses, "a");

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

        for (i = 0; i < num_threads; i++) {
                pthread_join(workers[i], NULL);
        }

        fclose(f);

        return 0;
}

static void *
event_loop(queue_t *q)
{
        char         **args;
        int          id;
        char         *line;
        request_t    *r;
        queue_node_t *n;

        id = 1;

        while (!end) {
                printf("> ");

                fflush(stdout);

                line = read_line();
                args = split_line(line);

                gettimeofday(&t, NULL);

                r = malloc(sizeof(request_t));
                r->cmd = args; r->request_id = id;
                r->starttime = t;

                pthread_mutex_lock(&q_lock);

                enqueue(q, r);

                id++;

                pthread_cond_broadcast(&worker_cv);

                pthread_mutex_unlock(&q_lock);
        }

        printf("end %d\n", end);

        return NULL;
}

static void *
handle_request_thread(void *arg)
{
        while (!end) {
                queue_t *q = (queue_t *) arg;
                queue_node_t *n;
                request_t *r;

                pthread_mutex_lock(&q_lock);

                while (is_empty(q))
                        pthread_cond_wait(&worker_cv, &q_lock);


                printf("---Handling request---\n");

                n = dequeue(q);

                r = n->datum;

                char **args = r->cmd;


                if (strncasecmp(args[0], "CHECK", 5 ) == 0) {
                        printf("---Handling balance check---\n");
                        handle_balance_check(args, n);
                } else if (strncasecmp(args[0], "TRANS", 5 ) == 0) {
                        printf("Handling transaction\n");
                        handle_trans(args, n);
                } else if (strncasecmp(args[0], "END", 3 ) == 0) {
                        printf("Handling exit\n");
                        end = handle_exit();
                } else {
                        printf("Invalid input: %s\n", r->cmd[0]);
                }

                pthread_mutex_unlock(&q_lock);
        }
        return NULL;
}


static uint8_t
handle_balance_check(char **argv, queue_node_t *n)
{
        int balance;
        int acc_id;
        int req_id;
        char *message;
        request_t *r;

        r = (request_t *)(n->datum);

        acc_id = atoi(argv[1]);

        pthread_mutex_lock(&accounts[acc_id].lock);

        balance = read_account(acc_id);

        pthread_mutex_unlock(&accounts[acc_id].lock);

        req_id = ((request_t *) n->datum)->request_id;

        printf("< ID %d\n", req_id);

        message = malloc(sizeof(char) * 50);

        gettimeofday(&t, NULL);

        r->endtime = t;

        flockfile(f);

        fprintf(f, "%d BAL %d TIME %ld.%06.ld %ld.%06.ld \n", req_id, balance, (long) r->starttime.tv_sec,
                (long) r->starttime.tv_usec, (long) r->endtime.tv_sec, (long) r->endtime.tv_usec);
        printf("%s\n", message);

        funlockfile(f);

        fflush(f);

        printf("----------\n");

        return 0;
}


/* trash ass code */
/* On success, return 1 */
uint8_t
handle_trans(char **argv, queue_node_t *n)
{
        int acc_id;
        int req_id;
        int i, j, k;
        int size;
        request_t *r;


        i = 1;
        size = 0;
        r = (request_t *)(n->datum);

        /* We're just doing this to get the size for now */
        while (r->cmd[i] != NULL) {
                size++; // could be the cause of some problems.
                i++; /* Iterator for our thing */
        }

        /* Size should be divisible by 2 with valid input */
        r->transactions = malloc(sizeof(transaction_t) * size/2);

        k = 0; // iterating the transactions array in the request
        j = 1; // iterating per account_id
        i = 2; // iterating per amount
        while (r->cmd[i] != NULL) {
                r->transactions[k].acc_id = atoi(r->cmd[j]);
                r->transactions[k].amount = atoi(r->cmd[i]);

                k++;
                j+=2;
                i+=2;
        }

        if (process_trans(r, k)) {
                printf("Error processing transactions");
                return 1;
        }

        printf("< ID %d\n", r->request_id);

        free (r->transactions);

        return 0;
}

int
process_trans(request_t *r, int trans_size)
{
        int i; // iterator for transactions
        int trans_amount; // amount we are sending or deducting
        int id; // id of particular transaction
        int acc_balance; // account balance of transacting acc
        int write_val; // the amount we are writing to the account
        transaction_t *tr = (r->transactions);

        for (i = 0; i < trans_size; i++) {
                pthread_mutex_lock(&accounts[i].lock);

                acc_balance = accounts[tr[i].acc_id].value; // we assume acc list is origanized by id

                trans_amount = tr[i].amount; // amount recorded from trans

                id = tr[i].acc_id; // id recorded from trans

                /* Check if the account has enough funds to withdrawal */

                // trans amount could be negative
                if (trans_amount < 0 && (acc_balance + trans_amount < 0)) {
                        printf("------Not enough funds in account %d-----\n", i);

                        flockfile(f);

                        fprintf(f, "%d ISF TIME %ld.%06.ld %ld.%06.ld \n", r->request_id, (long) r->starttime.tv_sec,
                                (long) r->starttime.tv_usec, (long) r->endtime.tv_sec, (long) r->endtime.tv_usec);

                        funlockfile(f);

                        fflush(f);

                        return 1;
                } else {
                        write_val = trans_amount + acc_balance;

                        write_account(id, write_val);

                        accounts[id].value = read_account(id);

                        gettimeofday(&t, NULL);

                        flockfile(f);

                        fprintf(f, "%d OK TIME %ld.%06.ld %ld.%06.ld \n", r->request_id, (long) r->starttime.tv_sec,
                               (long) r->starttime.tv_usec, (long) r->endtime.tv_sec, (long) r->endtime.tv_usec);

                        funlockfile(f);

                        fflush(f);
                }
                pthread_mutex_unlock(&accounts[i].lock);
        }

        return 0;
}

static uint8_t
handle_exit(void)
{
        printf("Exiting. . .\n");
        return 1;
}

/* For debugging */
static void
print_queue(queue_t *q)
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
        free(contents);
}

