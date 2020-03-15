#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"
#include "io.h"
#include "queue.h"

void event_loop(void);
void handle_balance_check(char **argv);
void handle_trans(char **argv);
void handle_exit(void);

int main(int argc, char **argv)
{
    num_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    responses = argv([3]);

    int num_accounts;
    int num_threads;

    const char *responses;


    FILE *f;
    f = fopen(responses, w);

    /* Initialize the accounts */
    if (initialize_accounts(num_accounts) != 1) {
        perror("Error: initialize accounts failed");
    }

    printf("%d accounts created.\n", num_accounts);

    /* Let's create the threads here */
    printf("%d threads created. \n", num_threads);

    event_loop(void);

    return 0;
}

void event_loop(void) {
    char *line;
    char **argv;

    do {
        printf("> ");
        line = readline();
        argv = split_line;

        request_t *r = malloc(sizeof(request));

        if (strncmp(argv[0], "CHECK", 2 * sizeof(char))) {
            // handle balance check
            handle_balance_check(argv, r);
        } else if (strncmp(argv[0], "TRANS", 5 * sizeof(char))) {
            // handle transaction
            handle_trans(argv, r);
        } else if (strncmp(argv[0], "END", 3 * sizeof(char))) {
            // handle exit
            handle_exit(void);
        } else {
            perror("Invalid input: %s\n", line);
        }

    } while (true);
}

/* On success, return 1 */
uint8_t handle_balance_check(char **argv, request_t *r)
{

    return 0;
}

/* On success, return 1 */
uint8_t handle_trans(char **argv, request_t *r);
{
    return 0;
}

void handle_exit(void);
{
}
