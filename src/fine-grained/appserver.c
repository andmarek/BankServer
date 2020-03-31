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
        pthread_t *workers;

//        setbuf(stdout, NULL); need ?

        end = 0;

        num_threads = atoi(argv[1]);
        workers = malloc(sizeof(pthread_t) * num_threads);

        num_accounts = atoi(argv[2]);

        responses = argv[3];
        printf("Writing to file: %s\n", responses);

        q = malloc(sizeof(queue_t));
        queue_init(q);

        pthread_mutex_init(&q_lock, NULL);

        pthread_cond_init(&worker_cv, NULL);

        pthread_create(&io, NULL, event_loop, q);


        printf("%d worker threads initialized\n", num_threads);
        printf("%d accounts initialized\n", num_accounts);
        fflush(stdout);

        for (i = 0; i < num_threads; i++) {
                pthread_create(&workers[i], NULL, handle_request_thread, q);
        }


        f = fopen(responses, "a");

        accounts = malloc(num_accounts * sizeof(account_t));

        if (!initialize_accounts(num_accounts)) {
                printf("Error: initialize accounts failed");
                return 1;
        }


        fflush(stdout);

        for (i = 0; i < num_accounts; i++) {
                pthread_mutex_init(&(accounts[i].lock), NULL);
                accounts[i].value = 0;
        }


        pthread_join(io, NULL);

        for (i = 0; i < num_threads; i++) {
                pthread_join(workers[i], NULL);
        }
        for (i = 0; i < num_threads; i++) {
            pthread_cancel(workers[i]);
        }
        /* test to see if it ends */
        if (end == 1) {
            printf("Okay have we reached this yet?");
            return 0;
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

        while (end == 0) {
                fflush(stdout);
                pthread_mutex_lock(&q_lock);

                line = read_line();
                args = split_line(line);


                if (strncasecmp(args[0], "END", 3) == 0) {
                    end = 1;
                    printf("we should be ending\n");
                    break;
                    pthread_cond_broadcast(&worker_cv);
                    pthread_mutex_unlock(&q_lock);
                    return 0;
                } 

                gettimeofday(&t, NULL);

                r = malloc(sizeof(request_t));
                r->cmd = args; r->request_id = id;
                r->starttime = t;


                if (end == 1) {
                    return 0;
                }

                enqueue(q, r);

                id++;

                pthread_cond_broadcast(&worker_cv);

                pthread_mutex_unlock(&q_lock);
        }
            if (end == 1) {
                printf("end is 1 in event loop\n");
                    return 0;
            }
        printf("end is here %d:\n", end);

        pthread_exit(0);
}

static void *
handle_request_thread(void *arg)
{
        queue_t *q = (queue_t *) arg;
        while (end == 0) {
                queue_node_t *n;
                request_t *r;

                pthread_mutex_lock(&q_lock);
                if (end == 1) {
                    return 0;
                }
                while (is_empty(q)) 
                        pthread_cond_wait(&worker_cv, &q_lock);
                
                if (end) {
                    printf("end detected \n");
                }

                n = dequeue(q);

                r = n->datum;

                char **args = r->cmd;

                if (strncasecmp(args[0], "CHECK", 5 ) == 0) {
                        handle_balance_check(args, n);
                } else if (strncasecmp(args[0], "TRANS", 5 ) == 0) {
                        handle_trans(args, n);
                } else if (strncasecmp(args[0], "END", 3 ) == 0) {
                        end = 1;
                        //return 0;
                        break;
                        printf("exit bro \n");
                } else {
                        printf("Invalid input: %s\n", r->cmd[0]);
                        continue;
                }
                printf("< ID %d\n", r->request_id);

                pthread_mutex_unlock(&q_lock);
        }

        if (end == 1) {
            printf("heyo\n");
            return 0;
        }
        return 0;
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

        message = malloc(sizeof(char) * 50);

        gettimeofday(&t, NULL);

        r->endtime = t;

        flockfile(f);

        fprintf(f, "%d BAL %d TIME %ld.%06ld %ld.%06ld\n",
                req_id,
                balance,
                (long) r->starttime.tv_sec,
                (long) r->starttime.tv_usec,
                (long) r->endtime.tv_sec,
                (long) r->endtime.tv_usec);

        funlockfile(f);

        fflush(f);

        return 0;
}


/* trash ass code */
/* On success, return 0 */
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
                size++; /* could be the cause of some problems. */
                i++; /* Iterator for our thing */
        }

        /* Size should be divisible by 2 with valid input */
        r->transactions = malloc(sizeof(transaction_t) * size/2);

        k = 0; /* iterating the transactions array in the request */
        j = 1; /* iterating per account_id */
        i = 2; /* iterating per trans amount */
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

        fflush(stdout);

        free (r->transactions);

        return 0;
}

int
process_trans(request_t *r, int trans_size)
{
        int i;
        int trans_amount;
        int id;
        int acc_balance;
        int write_val;
        transaction_t *tr;

        tr = (r->transactions);

        for (i = 0; i < trans_size; i++) {
                pthread_mutex_lock(&accounts[i].lock);

                acc_balance = accounts[tr[i].acc_id].value; /* We assume acc list is origanized by id */

                trans_amount = tr[i].amount; /* amount recorded from trans */

                id = tr[i].acc_id; /* id recorded from trans */

                /* Check if the account has enough funds to withdrawal */

                /* Trans amount could be negative */
                if (trans_amount < 0 && (acc_balance + trans_amount < 0)) {
                        printf("Not enough funds in account %d\n", i);

                        flockfile(f);

                        gettimeofday(&t, NULL);

                        r->endtime = t;

                        fprintf(f, "%d ISF TIME %ld.%06ld %ld.%06ld\n",
                                r->request_id,
                                (long) r->starttime.tv_sec,
                                (long) r->starttime.tv_usec,
                                (long) r->endtime.tv_sec,
                                (long) r->endtime.tv_usec);

                        funlockfile(f);

                        fflush(f);

                        return 1;
                } else {
                        write_val = trans_amount + acc_balance;

                        write_account(id, write_val);

                        accounts[id].value = read_account(id);

                        gettimeofday(&t, NULL);

                        r->endtime = t;

                }

                pthread_mutex_unlock(&accounts[i].lock);
        }

        flockfile(f);

        fprintf(f, "%d OK TIME %ld.%06ld %ld.%06ld\n",
                r->request_id,
                (long) r->starttime.tv_sec,
                (long) r->starttime.tv_usec,
                (long) r->endtime.tv_sec,
                (long) r->endtime.tv_usec);

        funlockfile(f);

        fflush(f);

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

