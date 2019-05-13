#include "sope.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>

/*
PLAN OF ATTACK:
1. server creates server fifo
2. server gets user information
3. server validates user information and creates user fifo
4. server gets user request
5. server validates user request
6. server sends requested information through user fifo
*/

void server_create() {
   printf("Creating FIFO communication channels...");

   if (mkfifo(SERVER_FIFO_PATH, 0666)) {
      printf("\nfifo_server: Create attempt failed\n");
      exit(RC_OTHER);
   }

   printf("\nChannels created.\nServer is online.\n");
}

void server_connect_user(int* fifo_server, int* fifo_user) {

   *fifo_user = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block

   printf("Opening FIFO communication channels...");
   // ---- opening server fifo -  does not wait for server
   *fifo_server = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_server == -1) {
      printf("\nfifo_server: Open attempt failed.\n");
      exit(RC_SRV_DOWN); 
   }

   /*
   // ---- get user fifo path name 
   char user_fifo_path[USER_FIFO_PATH_LEN] = USER_FIFO_PATH_PREFIX;
   char user_fifo_path_sufix[WIDTH_ID + 1];
   sprintf(user_fifo_path_sufix, "%d", getpid());
   strcat(user_fifo_path, user_fifo_path_sufix);
   printf("\nTESTING USER FIFO PATH: %s\n", user_fifo_path);

   // ---- creating user fifo
   if(mkfifo(user_fifo_path, 0666)) {
      printf("\nfifo_user: Create attempt failed\n");
   }
   // ---- opening user fifo - waits for server to connect to user
   *fifo_user = open(user_fifo_path, O_WRONLY);
   printf("here %d\n", *fifo_user);
   if (*fifo_user == -1) {
      printf("\nfifo_user: Open attempt failed\n");
      perror("error: ");
      exit(RC_USR_DOWN);
   }
   */

   sleep(10);

   printf("\nChannels opened.\nServer is connected to user <pid_here>.\n");
}

int main() {
   
   int fifo_user, fifo_server;

   server_create();

   server_connect_user(&fifo_server, &fifo_user);

   return 0;
}