#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bank.h"

int main(int argc, char **argv)
{
    // get user input
    int num_accounts;
    int num_threads;

    num_threads = atoi(argv[1]);
    num_accounts = atoi(argv[2]);
    
    if (initialize_accounts(num_accounts) != 1) {
        perror("Error: initialize accounts failed");
    }
    

    return 0;
}
