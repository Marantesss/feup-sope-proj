#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "queue.h"

/**
 * 
 */
bank_account_t accounts[MAX_BANK_ACCOUNTS];

/**
 * 
 */
int num_threads;

/**
 * 
 */
int logfile;

/**
 * 
 */
int shutdown_req_pid;

/**
 * 
 */
int num_active_threads = 0;

/**
 * 
 */
pthread_t thread_id[MAX_BANK_OFFICES];

/**
 * 
 */
int thread_arg[MAX_BANK_OFFICES];

/**
 * 
 */
pthread_mutex_t queue_mut = PTHREAD_MUTEX_INITIALIZER;

/**
 * 
 */
pthread_mutex_t counter_mut = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t log_mut = PTHREAD_MUTEX_INITIALIZER;

/**
 * 
 */
int shutdown = 0;

/**
 * 
 */
pthread_cond_t cond_queued_req = PTHREAD_COND_INITIALIZER;

/**
 * 
 */
queue_t request_queue;

/**
 * 
 */
void* thread_work(void * thread_id);

/**
 * 
 */
void acknowledge_request(tlv_request_t *req, tlv_reply_t *reply, int office_id);

/**
 * 
 */
int validate_request(tlv_request_t *req, tlv_reply_t *reply);

/**
 * 
 */
int validate_admin(req_header_t *req_header, rep_header_t* reply_header);

/**
 * 
 */
int validate_user(req_header_t *req_header, rep_header_t* reply_header);

/**
 * 
 */
void create_admin_account(char* password);

/**
 * 
 */
int create_user_account(req_create_account_t* create, rep_value_t* rep_value);

/**
 * 
 */
void check_user_balance(uint32_t id, rep_value_t* rep_value);

/**
 * 
 */
void create_user_transfer(uint32_t id, req_transfer_t* transfer, rep_value_t* rep_value);

/**
 * 
 */
void shutdown_server(rep_value_t* rep_value, int *fifo_request);

/**
 * 
 */
void server_fifo_create(int* fifo_request);

/**
 * 
 */
void user_fifo_create(int* fifo_reply, pid_t pid);

/**
 * 
 */
int read_request(int fd, tlv_request_t* req);

#endif
