#include "server.h"

int main(int argc, char *argv[]) {
   setbuf(stdout, NULL); // prints stuff without needing \n
   
   int fifo_user, fifo_server;

   printf("WARNING: No verifications are being made to command arguments!!!\n");

   if(argc != 3) {
      printf("Too few arguments\n");
      exit(EXIT_FAILURE);
   }

   num_threads = min(atoi(argv[1]), 10);

   create_admin_account(&accounts[0], argv[2]);

   server_fifo_create(&fifo_server);

   //while(1)
   server_connect_user(&fifo_server, &fifo_user);

   char ola[4];
   while(!readline(fifo_server, ola)) {
      sleep(1);
      printf("stuck\n");
   }
   printf("%s\n", ola);

   tlv_request_t req;
   while(!read_request(fifo_server, &req)) sleep(1);

   return 0;
}

void create_admin_account(bank_account_t* admin_account, char* password) {
   printf("CREATING ADMIN ACCOUNT...");
   // ---- checking if account credentials are valid
   // checking password
   size_t password_length = strlen(password);
   if (!between(MIN_PASSWORD_LEN, password_length, MAX_PASSWORD_LEN)) {
      printf("ERROR: Invalid password\n");
      exit(EXIT_FAILURE);
   }
   strtok(password, " ");
   if (strlen(password) != password_length) {
      printf("ERROR: Invalid password\n");
      exit(EXIT_FAILURE);
   }

   admin_account->account_id = ADMIN_ACCOUNT_ID;
   admin_account->balance = 0;
   // TODO generate random salt
   strcpy(admin_account->salt,"something");
   // TODO generate hash
   strcpy(admin_account->hash, strcat(password, admin_account->salt));
   
   printf("\nADMIN ACCOUNT CREATED.\n");
}

int create_user_account(bank_account_t* user_account, unsigned int id, unsigned int balance, char* password) {
   printf("CREATING USER ACCOUNT...");
   // ---- checking if account credentials are valid
   // checking id
   if (!between(1, id, MAX_BANK_ACCOUNTS)) {
      printf("ERROR: Invalid ID\n");
      return RC_OTHER;
   }
   for (int i = 0; i < MAX_BANK_ACCOUNTS; i++)
      if (id == accounts[i].account_id) {
         printf("ERROR: Invalid ID\n");
         return RC_ID_IN_USE;
      }
   // checking balance
   if (!between(MIN_BALANCE, balance, MAX_BALANCE)) {
      printf("ERROR: Invalid balance\n");
      return RC_OTHER;
   }
   // checking password
   size_t password_length = strlen(password);
   if (!between(MIN_PASSWORD_LEN, password_length, MAX_PASSWORD_LEN)) {
      printf("ERROR: Invalid password\n");
      return RC_OTHER;
   }
   strtok(password, " ");
   if (strlen(password) != password_length) {
      printf("ERROR: Invalid password\n");
      return RC_OTHER;
   }

   // ---- creating account
   user_account->account_id = id;
   user_account->balance = balance;
   // TODO generate random salt
   strcpy(user_account->salt,"something");
   // TODO generate hash
   strcpy(user_account->hash, strcat(password, user_account->salt));

   printf("\nUSER ACCOUNT CREATED.\n");

   return RC_OK;
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

int read_request(int fd, tlv_request_t* req) {
   int n;

   // reads pointer
   n = read(fd, req, 1);
   if (n != 0)
      printf("type: %d\tlenght: %d\tvalue: PID: %d\n", req->type, req->length, req->value.header.pid);

   return (n>0);
}
