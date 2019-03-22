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

int raised_flags[] = {0, 0, 0, 0};
char directory[MAX_FILE_NAME];
char outfile[MAX_FILE_NAME];
char cryptohash[MAX_FILE_NAME];

/*
file_name,file_type,file_size,file_access_owner,file_modification_da
te, file_access_date, md5,sha1,sha256
*/
static void dump_stat(struct stat *st) {
    struct tm *date;

    // ---- printing basic info
    printf("file_name, ");

    switch (st->st_mode & S_IFMT) {
    case S_IFBLK:
        printf("block device, ");
        break;
    case S_IFCHR:  
        printf("character device, ");
        break;
    case S_IFDIR:
        printf("directory, ");
        break;
    case S_IFIFO:
        printf("FIFO/pipe, ");
        break;
    case S_IFLNK:
        printf("symlink, ");
        break;
    case S_IFREG:
        printf("regular file, ");
        break;
    case S_IFSOCK:
        printf("socket, ");
        break;
    default:
        printf("unknown?, ");
        break;
    }

    // ---- getting permissions
    if (st->st_mode & S_IRUSR)
        printf("r");
    if (st->st_mode & S_IWUSR)
        printf("w");
    if (st->st_mode & S_IXUSR)
        printf("x");

    /*
    S_IRUSR  00400 user has read permission

    S_IWUSR  00200 user has write permission

    S_IXUSR  00100 user has execute permission
    */

    // ---- printing modification date
    date = gmtime(&st->st_mtime);
    printf(", %d-%02d-%02dT%02d:%02d:%02d, ", 1900 + date->tm_year, date->tm_mon, date->tm_mday,
                                            date->tm_hour, date->tm_min, date->tm_sec);

    // ---- printing access date
    date = gmtime(&st->st_atime);
    printf("%d-%02d-%02dT%02d:%02d:%02d\n", 1900 + date->tm_year, date->tm_mon, date->tm_mday,
                                            date->tm_hour, date->tm_min, date->tm_sec);
}

int main(int argc, char *argv[]) {   
    struct stat st;

    // ---- checking the maximum ammount of flags
    if (argc > 8 ) {
        printf("Error: Too many arguments");
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // ---- checking for help
    if (!strcmp(argv[1], "--help")) {
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        return 0;
    }

    // ---- parsing command flags
    for (int i = 1; i < argc - 1; i++) {
        // --- -r flag
        if (!strcmp(argv[i], "-r")) {
            raised_flags[RECURSIVE] = 1;
        // --- -h flag
        } else if (!strcmp(argv[i], "-h")) {
            raised_flags[CRYPTOHASH] = 1;
            i++;
            if (!(strcmp(argv[i], "-o") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(cryptohash, argv[i]);
        // --- -o flag
        } else if (!strcmp(argv[i], "-o")) {
            raised_flags[OUTFILE] = 1;
            i++;
            if (!(strcmp(argv[i], "-h") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(outfile, argv[i]);
        // --- -v flag
        } else if (!strcmp(argv[i], "-v")) {
            raised_flags[LOGFILE] = 1;
        } else {
            printf("unknown option: %s\n", argv[i]);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    // ---- getting directory
    strcpy(directory, argv[argc-1]);

    // ---- printing the flags: TEXTING PURPOSES
    if (raised_flags[RECURSIVE])
        printf("-r -> Analize all files\n");
    if (raised_flags[CRYPTOHASH])
        printf("-h -> Calculate one or more \"fingerprints\" with algorithm(s): %s\n", cryptohash);
    if (raised_flags[OUTFILE])
        printf("-o -> Store collected data on filename: %s\n", outfile);
    if (raised_flags[LOGFILE])
        printf("-v -> Create a log file\n");

    printf("Directory: %s\n", directory);

    // ---- getting info from directory
    if (stat(directory, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // ---- printing the info from directory
    dump_stat(&st);

    exit(EXIT_SUCCESS); 
}

