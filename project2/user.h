#include "sope.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
