#include "sope.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>

void user_connect_server(int* fifo_server, int* fifo_user) {

    printf("Opening FIFO communication channels...");
    // ---- opening server fifo
    *fifo_server = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
    *fifo_user = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
    /*
    if (*fifo_server == -1) {
        printf("\nfifo_server: Open attempt failed.\n");
        exit(RC_SRV_DOWN); 
    }
    */

    // ---- get user fifo path name 
    char user_fifo_path[USER_FIFO_PATH_LEN] = USER_FIFO_PATH_PREFIX;
    char user_fifo_path_sufix[WIDTH_ID + 1];
    sprintf(user_fifo_path_sufix, "%d", getpid());
    strcat(user_fifo_path, user_fifo_path_sufix);
    printf("\nTESTING USER FIFO PATH: %s\n", user_fifo_path);

    // ---- creating user fifo
    if(mkfifo(user_fifo_path, 0660)) {
        printf("\nfifo_user: Create attempt failed\n");
    }
    // ---- opening user fifo
    //*fifo_server=open(user_fifo_path, O_RDONLY);
    *fifo_user = open(user_fifo_path, O_WRONLY | O_NONBLOCK);
    if (*fifo_user == -1) {
        printf("\nfifo_user: Open attempt failed\n");
        exit(RC_USR_DOWN);
    }

    printf("\nChannels opened.\nUser %d connected to server.\n", getpid());
}

int main() {

    int fifo_user, fifo_server;

    user_connect_server(&fifo_user, &fifo_server);

    return 0;
}