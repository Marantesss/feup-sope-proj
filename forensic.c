#include "utils.h"

command_info *command = NULL; /**< @brief Struct containing command information*/

void print_fileinfo(FILE* print_location, file_info* info) {
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
    if (command->cryptohash_flags[MD5_FLAG]) {
        fwrite(",", sizeof(char), strlen(","), print_location);
        fwrite(info->md5_hash, sizeof(char), strlen(info->md5_hash), print_location);
    }
    // ---- file sha1 hash code
    if (command->cryptohash_flags[SHA1_FLAG]) {
        fwrite(",", sizeof(char), strlen(","), print_location);
        fwrite(info->sha1_hash, sizeof(char), strlen(info->sha1_hash), print_location);
    }
    // ---- file sha256 hash code
    if (command->cryptohash_flags[SHA256_FLAG]) {
        fwrite(",", sizeof(char), strlen(","), print_location);
        fwrite(info->sha256_hash, sizeof(char), strlen(info->sha256_hash), print_location);
    }
    fwrite("\n", sizeof(char), strlen("\n"), print_location);
}

void dump_stat(char* path, file_info *info) {
    struct stat st;
    struct tm *date;

    // ---- getting info from directory
    if (stat(path, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    
    // ---- getting file name
    char file_command[] = "file ";
    char o_command_file[1000];

    strcat(file_command, path);

    FILE *f1 = popen(file_command, "r");

    while (fgets(o_command_file, 10000, f1) != NULL) {
        // printf("%s", out);
    }
    
    pclose(f1);

    strcpy(info->file_name, strtok(o_command_file, " "));
    info->file_name[strlen(info->file_name) - 1] = NULL;
    // ----
    
    // ---- getting file type
    //strtok(NULL, " ");
    strcpy(info->file_type, strtok(NULL, "\n"));
    // ----

    // ---- getting size
    sprintf(info->file_size, "%ld", st.st_size);
    // ----

    // ---- getting permissions
    strcpy(info->file_access_owner, "");
    
    if (st.st_mode & S_IRUSR)
        strcat(info->file_access_owner, "r");
    if (st.st_mode & S_IWUSR)
        strcat(info->file_access_owner, "w");
    if (st.st_mode & S_IXUSR)
        strcat(info->file_access_owner, "x");
    // ----

    // ---- getting modification date
    date = gmtime(&st.st_mtime);
    sprintf(info->file_modification_date, "%d-%02d-%02dT%02d:%02d:%02d",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----

    // ---- getting access date
    date = gmtime(&st.st_atime);
    sprintf(info->file_access_date, "%d-%02d-%02dT%02d:%02d:%02d",
            1900 + date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
    // ----
 
    // ---- getting cryptohash
    // ---- md5
    if (command->cryptohash_flags[MD5_FLAG]) {
        char md5_command[] = "md5sum ";
        char o_command_md5[1000];

        strcat(md5_command, path);

        FILE *fa = popen(md5_command, "r");

        while (fgets(o_command_md5, 10000, fa) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fa);

        strcpy(info->md5_hash, strtok(o_command_md5, " "));
    }
    // ---- sha1
    if (command->cryptohash_flags[SHA1_FLAG]) {
        char sha1_command[] = "sha1sum ";
        char o_command_sha1[1000];

        strcat(sha1_command, path);

        FILE *fb = popen(sha1_command, "r");

        while (fgets(o_command_sha1, 10000, fb) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fb);

        strcpy(info->sha1_hash, strtok(o_command_sha1, " "));
    }
    // ---- sha256
    if (command->cryptohash_flags[SHA256_FLAG]) {
        char sha256_command[] = "sha256sum ";
        char o_command_sha256[1000];

        strcat(sha256_command, path);

        FILE *fc = popen(sha256_command, "r");

        while (fgets(o_command_sha256, 10000, fc) != NULL) {
            // printf("%s", out);
        }
    
        pclose(fc);

        strcpy(info->sha256_hash, strtok(o_command_sha256, " "));
    }
    // ----
}

void listdir(char* path, FILE* print_location, file_info* info) {
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(path);
    pid_t pid;

    if (!(dir = opendir(path))) {
        perror("in listdir() - path");
        exit(EXIT_FAILURE);
    }

    //fwrite(path, sizeof(char), strlen(path), print_location);
    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        // ---- directory is a folder and -r flag is turned on
        if (entry->d_type == DT_DIR) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            else if (command->raised_flags[RECURSIVE]) {
                pid = fork();
                // --- child
                if (pid == 0) {
                    if (path[len-1] != '/') {
                        path[len] = '/';
                        strcpy(path + len + 1, name);
                    } else {
                        strcpy(path + len, name);
                    }
                    listdir(path, print_location, info);
                    return;
                }
            }
        }
        // ---- directory is a file
        else {
            if (path[len-1] != '/') {
                path[len] = '/';
                strcpy(path + len + 1, name);
            } else {
                strcpy(path + len, name);
            }
            // ---- getting information from directory
            dump_stat(path, info);
            // ---- printing the info from directory
            print_fileinfo(print_location, info);
            path[len] = '\0';
        }
    }
    
    while (wait(NULL) != -1 && errno != ECHILD) {}

    closedir(dir);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int main(int argc, char *argv[]) {
    struct stat st;
    FILE * print_location;

    // ---- allocating memory for command_info and file_info
    file_info *info = calloc(1, sizeof(file_info));
    command = calloc(1, sizeof(command_info));

    // ---- checking the minimum ammount of argumrnts
    if (argc < 2 ) {
        printf("Error: Too few arguments\n");
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // ---- checking the maximum ammount of arguments
    if (argc > 8 ) {
        printf("Error: Too many arguments\n");
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // ---- checking for help
    if (!strcmp(argv[1], "--help")) {
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    // ---- parsing command flags
    for (int i = 1; i < argc - 1; i++) {
        // --- -r flag
        if (!strcmp(argv[i], "-r")) {
            command->raised_flags[RECURSIVE] = 1;
        // --- -h flag
        } else if (!strcmp(argv[i], "-h")) {
            command->raised_flags[CRYPTOHASH] = 1;
            i++;
            if (!(strcmp(argv[i], "-o") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(command->cryptohash, argv[i]);
        // --- -o flag
        } else if (!strcmp(argv[i], "-o")) {
            command->raised_flags[OUTFILE] = 1;
            i++;
            if (!(strcmp(argv[i], "-h") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            strcpy(command->outfile, argv[i]);
        // --- -v flag
        } else if (!strcmp(argv[i], "-v")) {
            command->raised_flags[LOGFILE] = 1;
            if (!(strcmp(argv[i], "-h") && strcmp(argv[i], "-o") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        } else {
            printf("unknown option: %s\n", argv[i]);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // ---- getting output location
    // "w" flag creates a file if it does not already exist
    // w --> O_WRONLY | O_CREAT | O_TRUNC
    if (command->raised_flags[OUTFILE])
        print_location = fopen(command->outfile, "w");
    else
        print_location = stdout;

    // ---- getting -h flags
    // Returns first token  
    char *token = strtok(command->cryptohash, ","); 
    
    while (token != NULL) {
        if (!strcmp(token, "md5"))
            command->cryptohash_flags[MD5_FLAG] = 1;
        else if (!strcmp(token, "sha1"))
            command->cryptohash_flags[SHA1_FLAG] = 1;
        else if (!strcmp(token, "sha256"))
            command->cryptohash_flags[SHA256_FLAG] = 1;
        else {
            printf("unknown option: %s\n", token);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, ","); 
    }     

    // ---- getting directory
    strcpy(command->directory, argv[argc-1]);

    // ---- making sure directory exists
    if(access(command->directory, F_OK) == -1) {
        printf("%s: no such file or directory\n", command->directory);
        exit(EXIT_FAILURE);
    }

    // ---- checking if directory is a file or a directory
    if (is_regular_file(command->directory)) {
        // ---- getting information from directory
        dump_stat(command->directory, info);
        // ---- printing the info from directory
        print_fileinfo(print_location, info);
    }
    else {
        // ---- looping through every file
        listdir(command->directory, print_location, info);
    }

    // ---- freeing memory after work is done
    free(info);
    free(command);

    exit(EXIT_SUCCESS); 
}
