#include "utils.h"

command_info *com;

void print_fileinfo(file_info* info, FILE print_location) {
    // prints in the console
    //fwrite("ola", 1, strlen("ola"), stdout); 
}

/*
file_name,file_type,file_size,file_access_owner,file_modification_da
te, file_access_date, md5,sha1,sha256
*/

static void dump_stat(struct stat *st) {
    struct tm *date;
    file_info *info = malloc(sizeof(file_info));
    // --- seeting default values
    strcpy(info->file_name, "");
    strcpy(info->file_type, "");
    info->file_size = 0;
    strcpy(info->file_access_owner, "");
    strcpy(info->file_modification_date, "");
    strcpy(info->file_access_date, "");
    strcpy(info->md5_hash, "");
    strcpy(info->sha1_hash, "");
    strcpy(info->sha256_hash, "");
    // ----
    
    // ---- getting file name
    char file_command[] = "file ";
    char o_command_file[1000];

    strcat(file_command, com->directory);

    FILE *f1 = popen(file_command, "r");

    while (fgets(o_command_file, 10000, f1) != NULL) {
        // printf("%s", out);
    }
    
    pclose(f1);

    strcpy(info->file_name, strtok(o_command_file, ": "));
    // ----
    
    // ---- getting file type
    strcpy(info->file_type, strtok(NULL, "\n"));
    // ----
    
    

    // ---- getting size
    info->file_size = st->st_size;
    // ----

    // ---- getting permissions
    if (st->st_mode & S_IRUSR)
        strcat(info->file_access_owner, "r");
    if (st->st_mode & S_IWUSR)
        strcat(info->file_access_owner, "w");
    if (st->st_mode & S_IXUSR)
        strcat(info->file_access_owner, "x");
    // ----

    // ---- getting modification date
    date = gmtime(&st->st_mtime);
    sprintf(info->file_modification_date, ", %d-%02d-%02dT%02d:%02d:%02d, ",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----

    // ---- getting access date
    date = gmtime(&st->st_atime);
    sprintf(info->file_access_date, "%d-%02d-%02dT%02d:%02d:%02d\n",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----
 
    // ---- getting cryptohash

    // ----
}

int main(int argc, char *argv[]) {
    struct stat st;

    com = malloc(sizeof(command_info));

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
            com->raised_flags[RECURSIVE] = 1;
        // --- -h flag
        } else if (!strcmp(argv[i], "-h")) {
            com->raised_flags[CRYPTOHASH] = 1;
            i++;
            if (!(strcmp(argv[i], "-o") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(com->cryptohash, argv[i]);
        // --- -o flag
        } else if (!strcmp(argv[i], "-o")) {
            com->raised_flags[OUTFILE] = 1;
            i++;
            if (!(strcmp(argv[i], "-h") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(com->outfile, argv[i]);
        // --- -v flag
        } else if (!strcmp(argv[i], "-v")) {
            com->raised_flags[LOGFILE] = 1;
        } else {
            printf("unknown option: %s\n", argv[i]);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    // ---- getting directory
    strcpy(com->directory, argv[argc-1]);

    // ---- printing the flags: TEXTING PURPOSES
    if (com->raised_flags[RECURSIVE])
        printf("-r -> Analize all files\n");
    if (com->raised_flags[CRYPTOHASH])
        printf("-h -> Calculate one or more \"fingerprints\" with algorithm(s): %s\n", com->cryptohash);
    if (com->raised_flags[OUTFILE])
        printf("-o -> Store collected data on filename: %s\n", com->outfile);
    if (com->raised_flags[LOGFILE])
        printf("-v -> Create a log file\n");

    printf("Directory: %s\n", com->directory);

    // ---- getting info from directory
    if (stat(com->directory, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // ---- printing the info from directory
    dump_stat(&st);

    exit(EXIT_SUCCESS); 
}

