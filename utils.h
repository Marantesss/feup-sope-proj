#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>

// ---- Command flags
#define FLAG_NUMBER     4

#define RECURSIVE       0
#define CRYPTOHASH      1
#define OUTFILE         2
#define LOGFILE         3
// ---- cryptohash
#define MAX_CRYPTOHASH_FLAGS    3

#define MD5_FLAG                0
#define SHA1_FLAG               1
#define SHA256_FLAG             2

#define MD5_HASH_SIZE       32
#define SHA1_HASH_SIZE      40
#define SHA256_HASH_SIZE    64
// ----

#define MAX_FILE_NAME       64
#define ACCESS_PERMISSIONS  3

#define PID_NUMBER 8

typedef struct {
    char file_name[MAX_FILE_NAME+1];              /**< @brief  */
    char file_type[MAX_FILE_NAME+1];              /**< @brief  */
    char file_size[MAX_FILE_NAME+1];
    
    char file_access_owner[ACCESS_PERMISSIONS+1];
    char file_modification_date[MAX_FILE_NAME+1];
    char file_access_date[MAX_FILE_NAME+1];

    char md5_hash[MD5_HASH_SIZE+1];
    char sha1_hash[SHA1_HASH_SIZE+1];
    char sha256_hash[SHA256_HASH_SIZE+1];

    char process_id[PID_NUMBER+1];
} file_info;

typedef struct {
    char directory[MAX_FILE_NAME+1];
    char outfile[MAX_FILE_NAME+1];
    char cryptohash[MAX_FILE_NAME+1];
    int cryptohash_flags[MAX_CRYPTOHASH_FLAGS+1];
    int raised_flags[FLAG_NUMBER+1];
    char logfilename[MAX_FILE_NAME+1];
} command_info;

