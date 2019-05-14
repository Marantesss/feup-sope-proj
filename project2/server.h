#include "sope.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>

/**
 * 
 */
#define min(a, b) (a) < (b) ? (a) : (b)
/**
 * 
 */
#define between(min, num, max) ((min) <= (num) && (num) <= (max)) ? 1 : 0 


/**
 * 
 */
bank_account_t accounts[MAX_BANK_ACCOUNTS];


/**
 * 
 */
void create_admin_account(bank_account_t* admin_account, char* password);

/**
 * 
 */
int create_user_account(bank_account_t* user_account, unsigned int id, unsigned int balance, char* password);

/**
 * 
 */
void server_fifo_create(int* fifo_server);

/**
 * 
 */
void server_connect_user(int* fifo_server, int* fifo_user);

/**
 * 
 */
void user_fifo_create(char* user_fifo_path);

/**
 * 
 */
int readline(int fd, char *str);
