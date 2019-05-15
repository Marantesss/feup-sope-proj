#include "utils.h"

/**
 * 
 */
int readline(int fd, char *str);

/**
 * 
 */
void user_connect_server(int* fifo_server, int* fifo_user);

/**
 * 
 */
void get_user_fifo_path(char* user_fifo_path);

void create_request(tlv_request_t* req);

