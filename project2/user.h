#ifndef USER_H
#define USER_H

#include "utils.h"

/**
 * 
 */
void user_connect_server(int* fifo_request);

/**
 * 
 */
void user_connect_fifo_reply(int *fifo_reply, char* user_fifo_path);

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
int print_reply(tlv_reply_t* reply);

#endif
