#ifndef REQUEST_H
#define REQUEST_H

#include <time.h>

#include "transaction.h"

typedef struct request {
    struct request *next;
    int request_id; // r_id assigned by main thread
    int check_acc_id; // acc_id for CHECK request
    transaction_t *transactions; // array of the transaction data
    int num_trans; // number of accounts in transaction
    struct timeval starttime, endtime; // starttime and endtime for TIME
} request_t

#endif
