#include "user.h"

int main() {
   setbuf(stdout, NULL); // prints stuff without needing \n

   int fifo_user, fifo_server;

   user_connect_server(&fifo_server, &fifo_user);

   write(fifo_server, "ola", strlen("ola") + 1);

   tlv_request_t req;
   create_request(&req);

   write(fifo_server, &req, sizeof(tlv_request_t));
   
   printf("req: %ld\t&req: %ld\n", sizeof(req), sizeof(&req));

   return 0;
}

void get_user_fifo_path(char* user_fifo_path) {
   strcpy(user_fifo_path, USER_FIFO_PATH_PREFIX);
   char user_fifo_path_sufix[WIDTH_ID + 1];
   sprintf(user_fifo_path_sufix, "%d", getpid());
   strcat(user_fifo_path, user_fifo_path_sufix);
}

void user_connect_server(int* fifo_server, int* fifo_user) {
   printf("Opening FIFO communication channels...");

   // ---- opening server fifo - does not wait for server
   *fifo_server = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_server == -1) {
      printf("\nfifo_server: Open attempt failed.\n");
      exit(-1); 
   }

   // ---- opening user fifo - waits for server to create fifo and connect to user
   // get user fifo path name 
   char user_fifo_path[USER_FIFO_PATH_LEN];
   get_user_fifo_path(user_fifo_path);
   // sends user information to server
   write(*fifo_server, user_fifo_path, strlen(user_fifo_path) + 1);
   // wait for user fifo to be created
   while(access(user_fifo_path, F_OK)) sleep(1);
   // opening user fifo - waits for server to connect to user
   *fifo_user = open(user_fifo_path, O_RDONLY);
   if (*fifo_user == -1) {
      printf("\nfifo_user: Open attempt failed\n");
      exit(-1);
   }

   printf("\nChannels opened.\nUser %d connected to server.\n", getpid());
}

int readline(int fd, char *str) {
   int n;

   do { 
      n = read(fd, str, 1); 
   } while (n>0 && *str++ != '\0');

   return (n>0); 
}

void create_request(tlv_request_t* req) {
   // ---- type
   req->type = OP_CREATE_ACCOUNT;
   // ---- value
   // header
   req->value.header.account_id = 0;
   req->value.header.op_delay_ms = 10;
   strcpy(req->value.header.password,"olaolaol");
   req->value.header.pid = getpid();
   // create
   req->value.create.account_id = 2;
   req->value.create.balance = 100;
   strcpy(req->value.create.password, "ola");
   // ---- length
   req->length = sizeof(req->value);
}
