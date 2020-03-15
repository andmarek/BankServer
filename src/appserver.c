#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"
#include "io.h"
#include "request.h"
#include "transaction.h"
#include "queue.h"

void event_loop(void);
uint8_t handle_balance_check(char **argv, request_t *);
uint8_t handle_trans(char **argv, request_t *);
void handle_exit(void);

int main(int argc, char **argv)
{
    int num_threads, num_accounts;
    const char *responses;

    num_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    responses = argv[3];

    FILE *f;
    f = fopen(responses, "w");

    /* Initialize the accounts */
    if (initialize_accounts(num_accounts) != 1) {
        perror("Error: initialize accounts failed");
    }

    printf("%d accounts created.\n", num_accounts);

    /* Let's create the threads here */
    printf("%d threads created. \n", num_threads);

    event_loop();

    return 0;
}

void event_loop() {
    char *line;
    char **argv;

    do {
        printf("> ");
        line = read_line();
        argv = split_line(line);

        request_t *r = malloc(sizeof(request_t));

        if (strncmp(argv[0], "CHECK", 2 * sizeof(char))) {
            // handle balance check
            printf("Handling balance check\n");
            handle_balance_check(argv, r);
        } else if (strncmp(argv[0], "TRANS", 5 * sizeof(char))) {
            // handle transaction
            printf("Handling transaction\n");
            handle_trans(argv, r);
        } else if (strncmp(argv[0], "END", 3 * sizeof(char))) {
            // handle exit
            printf("Handling exit\n");
            handle_exit();
        } else {
            printf("Invalid input: %s\n", line);
        }

    } while (1);
}

/* On success, return 1 */
uint8_t handle_balance_check(char **argv, request_t *r)
{

    return 0;
}

/* On success, return 1 */
uint8_t handle_trans(char **argv, request_t *r)
{
    return 0;
}

void handle_exit(void)
{
}
