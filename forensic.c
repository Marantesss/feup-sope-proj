#include "utils.h"

command_info *com;
file_info *info;

void print_fileinfo(FILE* print_location) {
    /*
    file_name,file_type,file_size,file_access_owner,
    file_modification_date, file_access_date, md5,sha1,sha256
    */
    // ---- file name
    fwrite(info->file_name, sizeof(char), strlen(info->file_name), print_location);
    fwrite(",", sizeof(char), strlen(","), print_location);
    // ---- file type
    fwrite(info->file_type, sizeof(char), strlen(info->file_type), print_location);
    fwrite(",", sizeof(char), strlen(","), print_location);
    // ---- file size
    fwrite(info->file_size, sizeof(char), strlen(info->file_size), print_location);
    fwrite(",", sizeof(char), strlen(","), print_location);
    // ---- file access owner permissions
    fwrite(info->file_access_owner, sizeof(char), strlen(info->file_access_owner), print_location);
    fwrite(",", sizeof(char), strlen(","), print_location);
    // ---- file last modification date
    fwrite(info->file_modification_date, sizeof(char), strlen(info->file_modification_date), print_location);
    fwrite(",", sizeof(char), strlen(","), print_location);
    // ---- file last access date
    fwrite(info->file_access_date, sizeof(char), strlen(info->file_access_date), print_location);

    // ---- file md5 hash code
    if (com->cryptohash_flags[MD5_FLAG]) {
        fwrite(",", sizeof(char), strlen(","), print_location);
        fwrite(info->md5_hash, sizeof(char), strlen(info->md5_hash), print_location);
    }
    // ---- file sha1 hash code
    if (com->cryptohash_flags[SHA1_FLAG]) {
        fwrite(",", sizeof(char), strlen(","), print_location);
        fwrite(info->sha1_hash, sizeof(char), strlen(info->sha1_hash), print_location);
    }
    // ---- file sha256 hash code
    if (com->cryptohash_flags[SHA256_FLAG]) {
        fwrite(info->sha256_hash, sizeof(char), strlen(info->sha256_hash), print_location);
    }
    fwrite("\n", sizeof(char), strlen("\n"), print_location);
}

static void dump_stat(struct stat *st) {
    struct tm *date;
    info = calloc(1, sizeof(file_info));
    // --- seeting default values
    /*
    strcpy(info->file_name, "");
    strcpy(info->file_type, "");
    strcpy(info->file_size, "");
    strcpy(info->file_access_owner, "");
    strcpy(info->file_modification_date, "");
    strcpy(info->file_access_date, "");
    strcpy(info->md5_hash, "");
    strcpy(info->sha1_hash, "");
    strcpy(info->sha256_hash, "");
    */
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
    sprintf(info->file_size, "%ld", st->st_size);
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
    sprintf(info->file_modification_date, "%d-%02d-%02dT%02d:%02d:%02d",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----

    // ---- getting access date
    date = gmtime(&st->st_atime);
    sprintf(info->file_access_date, "%d-%02d-%02dT%02d:%02d:%02d",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----
 
    // ---- getting cryptohash
    // ---- md5
    if (com->cryptohash_flags[MD5_FLAG]) {
        char md5_command[] = "md5sum ";
        char o_command_md5[1000];

        strcat(md5_command, com->directory);

        FILE *fa = popen(md5_command, "r");

        while (fgets(o_command_md5, 10000, fa) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fa);

        strcpy(info->md5_hash, strtok(o_command_md5, " "));
    }
    // ---- sha1
    if (com->cryptohash_flags[SHA1_FLAG]) {
        char sha1_command[] = "sha1sum ";
        char o_command_sha1[1000];

        strcat(sha1_command, com->directory);

        FILE *fb = popen(sha1_command, "r");

        while (fgets(o_command_sha1, 10000, fb) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fb);

        strcpy(info->sha1_hash, strtok(o_command_sha1, " "));
    }
    // ---- sha256
    if (com->cryptohash_flags[SHA256_FLAG]) {
        char sha256_command[] = "sha256sum ";
        char o_command_sha256[1000];

        strcat(sha256_command, com->directory);

        FILE *fc = popen(sha256_command, "r");

        while (fgets(o_command_sha256, 10000, fc) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fc);

        strcpy(info->sha256_hash, strtok(o_command_sha256, " "));
    }
    // ----
}

int main(int argc, char *argv[]) {
    struct stat st;
    FILE * output;

    com = calloc(1, sizeof(command_info));
    /*
    strcpy(com->directory, "");
    strcpy(com->outfile, "");
    strcpy(com->cryptohash, "");
    ...
    ...
    */

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

    // ---- getting output location
    // "w" flag creates a file if it does not already exist
    // w --> O_WRONLY | O_CREAT | O_TRUNC
    if (com->raised_flags[OUTFILE])
        output = fopen(com->outfile, "w");
    else
        output = stdout;

    // ---- getting -h flags
    // Returns first token  
    char *token = strtok(com->cryptohash, ","); 
    
    while (token != NULL) {
        if (!strcmp(token, "md5"))
            com->cryptohash_flags[MD5_FLAG] = 1;
        else if (!strcmp(token, "sha1"))
            com->cryptohash_flags[SHA1_FLAG] = 1;
        else if (!strcmp(token, "sha256"))
            com->cryptohash_flags[SHA256_FLAG] = 1;
        else {
            printf("unknown option: %s\n", token);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, ","); 
    }     

    // ---- getting directory
    strcpy(com->directory, argv[argc-1]);

    // ---- printing the flags: TESTING PURPOSES
    /*
    if (com->raised_flags[RECURSIVE])
        printf("-r -> Analize all files\n");
    if (com->raised_flags[CRYPTOHASH])
        printf("-h -> Calculate one or more \"fingerprints\" with algorithm(s): %s\n", com->cryptohash);
    if (com->raised_flags[OUTFILE])
        printf("-o -> Store collected data on filename: %s\n", com->outfile);
    if (com->raised_flags[LOGFILE])
        printf("-v -> Create a log file\n");

    printf("Directory: %s\n", com->directory);
    */

    // ---- getting info from directory
    if (stat(com->directory, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // ---- printing the info from directory
    dump_stat(&st);

    print_fileinfo(output);

    exit(EXIT_SUCCESS); 
}

