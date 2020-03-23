#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 64

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
char *read_line()
{
    char *line;
    line = NULL;

    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);

    return line;
}

char **split_line(char *line)
{
    char *sep = " \t\r\n\a";
    char **tokens;
    char *token;

    int token_count = 0;

    tokens = malloc(sizeof (char*) * BUFSIZE);
    token = strtok(line, sep);

    while (token != NULL) {
        tokens[token_count] = token;
        token = strtok(NULL, sep);
        token_count++;
    }

    /* Putting a null byte here so we can count the argv eventually */
    tokens[token_count] = NULL;

    return tokens;
}

#endif
