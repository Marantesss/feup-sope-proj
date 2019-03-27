#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define RECURSIVE   0
#define CRYPTOHASH  1
#define OUTFILE     2
#define LOGFILE     3

#define MAX_FILE_NAME   64
#define ACCESS_PERMISSIONS 3

#define MD5_HASH_SIZE     16
#define SHA1_HASH_SIZE    20
#define SHA256_HASH_SIZE  64

#define FLAG_NUMBER     4

typedef struct {
    char file_name[MAX_FILE_NAME];              /**< @brief  */
    char file_type[MAX_FILE_NAME];              /**< @brief  */
    size_t file_size;
    
    char file_access_owner[ACCESS_PERMISSIONS];
    char file_modification_date[MAX_FILE_NAME];
    char file_access_date[MAX_FILE_NAME];

    char md5_hash[MD5_HASH_SIZE];
    char sha1_hash[SHA1_HASH_SIZE];
    char sha256_hash[SHA256_HASH_SIZE];
} file_info;

typedef struct {
    char directory[MAX_FILE_NAME];
    char outfile[MAX_FILE_NAME];
    char cryptohash[MAX_FILE_NAME];
    int raised_flags[FLAG_NUMBER];
} command_info;

