#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct stat sb;
    struct tm *date;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

     if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // ---- printing basic info
    printf("file_name, ");

    switch (sb.st_mode & S_IFMT) {
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
    if (sb.st_mode & S_IRUSR)
        printf("r");
    if (sb.st_mode & S_IWUSR)
        printf("w");
    if (sb.st_mode & S_IXUSR)
        printf("x");

    /*
    S_IRUSR  00400 user has read permission

    S_IWUSR  00200 user has write permission

    S_IXUSR  00100 user has execute permission
    */

    // ---- printing modification date
    date = gmtime(&sb.st_mtime);
    printf(", %d-%02d-%02dT%02d:%02d:%02d, ", 1900 + date->tm_year, date->tm_mon, date->tm_mday,
                                date->tm_hour, date->tm_min, date->tm_sec);

    // ---- printing access date
    date = gmtime(&sb.st_atime);
    printf("%d-%02d-%02dT%02d:%02d:%02d\n", 1900 + date->tm_year, date->tm_mon, date->tm_mday,
                                date->tm_hour, date->tm_min, date->tm_sec);
    
    /*
    printf("I-node number: %ld\n", (long) sb.st_ino);

    printf("Mode: %lo (octal)\n", (unsigned long) sb.st_mode);

    printf("Link count: %ld\n", (long) sb.st_nlink);
    printf("Ownership: UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);

    printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    printf("File size: %lld bytes\n", (long long) sb.st_size);
    printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);

    printf("Last status change: %s", ctime(&sb.st_ctime));
    printf("Last file access: %s", ctime(&sb.st_atime));
    printf("Last file modification: %d", cimte(&sb.st_mtime));
    */

   exit(EXIT_SUCCESS);
}