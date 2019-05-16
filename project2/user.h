#include "utils.h"

/**
 * 
 */
void user_connect_server(int* fifo_server);

/**
 * 
 */
void user_connect_user_fifo(int* fifo_user);

/**
 * 
 */
void get_user_fifo_path(char* user_fifo_path);

/**
 * 
 */
void get_request(char *argv[], tlv_request_t* request);

/**
 * 
 */
int read_reply(int fd, tlv_reply_t* reply);

/**
 * 
 */
int readline(int fd, char *str);

/**
 * TO DELETE
 */
void create_test_request(tlv_request_t* req);
