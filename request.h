#ifndef REQUEST_H 
#define REQUEST_H 
typedef struct request {
    struct job *next;
    int request_id;
    int check_acc_id;
    transaction *transactions;
    int num_trans;
    struct timeval starttime, endtime;
} request_t
#endif
