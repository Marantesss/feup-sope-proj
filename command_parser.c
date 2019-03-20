#include <stdio.h>  
#include <unistd.h>  

// forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>

int main(int argc, char *argv[])  
{ 
    int opt; 
      
    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'  
    while((opt = getopt(argc, argv, ":rh:o:v:")) != -1)  
    {  
        switch(opt)  
        {  
            case 'r':
                printf("Analise all files in directory: %c\n", opt);  
                break;  
            case 'h':
                printf("Calculate one or more fingerprints (%c) with algorithm(s): %s\n", opt, optarg);  
                break;
            case 'o':
                printf("Store collected data (%c) on filename: %s\n", opt, optarg);  
                break;
            case 'v':  
                printf("Create a log file (%c) with name %s\n", opt, optarg);  
                break;  
            case ':':  
                printf("option needs a value\n");  
                break;
            case '?':  
                printf("unknown option: %c\n", optopt); 
                break;  
        }  
    }  
      
    // optind is for the extra arguments 
    // which are not parsed 
    for(; optind < argc; optind++){      
        printf("extra arguments: %s\n", argv[optind]);  
    } 
      
    return 0; 
}

