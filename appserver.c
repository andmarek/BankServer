#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"


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
    
    if (initialize_accounts(num_accounts) != 1) {
        perror("Error: initialize accounts failed");
    }
    

    return 0;
}
