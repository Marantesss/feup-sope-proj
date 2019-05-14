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

int readline(int fd, char *str);
void server_fifo_create(int* fifo_server);
void user_fifo_create(char* user_fifo_path);
void server_connect_user(int* fifo_server, int* fifo_user);

int main() {
   setbuf(stdout, NULL); // prints stuff without needing \n
   
   int fifo_user, fifo_server;

   server_fifo_create(&fifo_server);

   while(1)
      server_connect_user(&fifo_server, &fifo_user);

   return 0;
}

void server_fifo_create(int* fifo_server) {
   printf("Creating server FIFO communication channels...");

   // ---- creating server fifo
   if (mkfifo(SERVER_FIFO_PATH, 0666)) {
      // If the file already exists, delete it
      unlink(SERVER_FIFO_PATH);
      // Try again
      if (mkfifo(SERVER_FIFO_PATH, 0666)) {
         printf("\nfifo_server: Create attempt failed\n");
         exit(-1);
      }
   }

   // ---- opening server fifo
   *fifo_server = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_server == -1) {
      printf("\nfifo_server: Open attempt failed.\n");
      exit(-1);
   }

   printf("\nChannel created and connected.\nServer is online.\n");
}

void user_fifo_create(char* user_fifo_path) {
   printf("\nCreating user FIFO communication channels...");

   // ---- creating user fifo
   if (mkfifo(user_fifo_path, 0666)) {
      // If the file already exists, delete it
      unlink(user_fifo_path);
      // Try again
      if (mkfifo(user_fifo_path, 0666)) {
         printf("\nfifo_server: Create attempt failed\n");
         exit(-2);
      }
   }

   printf("\n%s channel created.\n", user_fifo_path);
}

void server_connect_user(int* fifo_server, int* fifo_user) {
   // ---- get user fifo path name 
   printf("\nWaiting for user data...");   
   char user_fifo_path[USER_FIFO_PATH_LEN];
   // wait for server fifo to have something
   while(!readline(*fifo_server, user_fifo_path)) sleep(1);

   // ---- create user fifo
   user_fifo_create(user_fifo_path);

   printf("\nOpening FIFO communication channels...");
   // ---- opening user fifo - waits for user to connect to server
   *fifo_user = open(user_fifo_path, O_WRONLY);
   if (*fifo_user == -1) {
      printf("\nfifo_user: Open attempt failed\n");
      exit(-2);
   }

   printf("\nChannels opened.\nServer is connected to user <user_pid_here>.\n");
}

int readline(int fd, char *str) {
   int n;

   do { 
      n = read(fd, str, 1);
   } while (n > 0 && *str++ != '\0');

   return (n>0); 
}
