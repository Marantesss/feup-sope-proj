#include "utils.h"

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
void acknowledge_request(tlv_request_t *req, tlv_reply_t *reply);

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
void create_user_account(req_create_account_t* create, rep_value_t* rep_value);

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
void shutdown_server(rep_value_t* rep_value);

/**
 * 
 */
void server_fifo_create(int* fifo_server);

/**
 * 
 */
void user_fifo_create(int* fifo_user, pid_t pid);

/**
 * 
 */
int readline(int fd, char *str);

/**
 * 
 */
int read_request(int fd, tlv_request_t* req);
