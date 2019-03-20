#include <stdio.h>  
#include <unistd.h>
#include <string.h>

#define RECURSIVE   0
#define CRYPTOHASH  1
#define OUTFILE     2
#define LOGFILE     3

#define MAX_FILE_NAME   64

int raised_flags[] = {0, 0, 0, 0};
char directory[MAX_FILE_NAME];
char outfile[MAX_FILE_NAME];
char cryptohash[MAX_FILE_NAME];

int main(int argc, char *argv[]) {   
    // ---- checking the maximum ammount of flags
    if (argc > 8 ) {
        printf("Error: Too many arguments");
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        return 1;
    }
    // ---- checking for help
    if (!strcmp(argv[1], "--help")) {
        printf("Usage: %s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
        return 0;
    }

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
                return 1;
            }
            strcpy(cryptohash, argv[i]);
        // --- -o flag
        } else if (!strcmp(argv[i], "-o")) {
            raised_flags[OUTFILE] = 1;
            i++;
            if (!(strcmp(argv[i], "-h") && strcmp(argv[i], "-v") && strcmp(argv[i], "-r"))) {
                printf("Option %s needs a value\n", argv[i-1]);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                return 1;
            }
            strcpy(outfile, argv[i]);
        // --- -v flag
        } else if (!strcmp(argv[i], "-v")) {
            raised_flags[LOGFILE] = 1;
        } else {
            printf("unknown option: %s\n", argv[i]);
            printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
            return 1;
        }
    }

    /*
    int opt;
    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'
    // ---- Parsing the command flags
    while((opt = getopt(argc, argv, ":rh:o:v:")) != -1) {  
        switch(opt) {  
            case 'r':
                //printf("Analize all files (%c)\n", opt);
                raised_flags[RECURSIVE] = 1;
                break;
            case 'h':
                //printf("Calculate one or more \"fingerprints\" (%c) with algorithm(s): %s\n", opt, optarg);
                raised_flags[CRYPTOHASH] = 1;
                if (!(strcmp(optarg, "-o") && strcmp(optarg, "-v") && strcmp(optarg, "-r"))) {
                    printf("Option -%c needs a value\n", opt);
                    printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                    return 1;
                }
                strcpy(cryptohash, optarg);
                break;
            case 'o':
                //printf("Store collected data (%c) on filename: %s\n", opt, optarg);
                raised_flags[OUTFILE] = 1;
                if (!(strcmp(optarg, "-h") && strcmp(optarg, "-v") && strcmp(optarg, "-r"))) {
                    printf("Option -%c needs a value\n", opt);
                    printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                    return 1;
                }
                strcpy(outfile, optarg);
                break;
            case 'v':
                //printf("Create a log file (%c)\n", opt);
                raised_flags[LOGFILE] = 1;
                break;
            case ':':
                printf("Option needs a value\n");
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                return 1;
                break;
            case '?':
                printf("unknown option: -%c", opt);
                printf("Usage:\n%s [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n", argv[0]);
                return 1;
        }  
    }
    */
    
    
    strcpy(directory, argv[argc-1]);

    // ---- printing the flags
    if (raised_flags[RECURSIVE])
        printf("-r -> Analize all files\n");
    if (raised_flags[CRYPTOHASH])
        printf("-h -> Calculate one or more \"fingerprints\" with algorithm(s): %s\n", cryptohash);
    if (raised_flags[OUTFILE])
        printf("-o -> Store collected data on filename: %s\n", outfile);
    if (raised_flags[LOGFILE])
        printf("-v -> Create a log file\n");

    printf("Directory: %s\n", directory);

    return 0; 
}

