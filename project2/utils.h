#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "sope.h"


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
#define is_admin(id) id == 0 ? 1 : 0

/**
 * 
 */
char *rand_string(char *str, size_t size);

#endif
