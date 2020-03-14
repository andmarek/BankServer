#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"


request *parse_request(char **input)
{
    char *sep = " \t\r\n\a"
    request *r = malloc(sizeof request);
    strtok();

}

int main(int argc, char **argv)
{
    // get user input
    int num_accounts;
    int num_threads;

    const char *responses;

    num_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    responses = argv([3]);

    FILE *f;
    f = fopen(responses, w);

    /* Initialize the accounts */
    if (initialize_accounts(num_accounts) != 1) {
        perror("Error: initialize accounts failed");
    }

    printf("%d accounts created.\n", num_accounts);

    /* Let's create the threads here */
    printf("%d threads created. \n", num_threads);



    return 0;
}
