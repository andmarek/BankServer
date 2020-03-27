#ifndef UTILS_H
#define UTILS_H

typedef struct account {
    pthread_mutex_t lock;
    int value;
} account_t;

typedef struct transaction {
    int acc_id;
    int amount;
} transaction_t;

typedef struct request {
    char **cmd;
    struct request *next;
    int request_id; // r_id assigned by main thread
    int check_acc_id; // acc_id for CHECK request
    transaction_t *transactions; // array of the transaction data
    int num_trans; // number of accounts in transaction
    struct timeval starttime, endtime; // starttime and endtime for TIME
} request_t;

/* IO */
char *read_line(void);
char **split_line(char *);

#endif
