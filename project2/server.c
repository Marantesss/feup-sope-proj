#include "server.h"

int main(int argc, char *argv[]) {
   setbuf(stdout, NULL); // prints stuff without needing \n
   
   int fifo_user, fifo_server;

   printf("WARNING: No verifications are being made to command arguments!!!\n");

   if(argc != 3) {
      printf("Too few arguments\n");
      exit(EXIT_FAILURE);
   }

   // maybe not necessary
   //memset(accounts, 0, MAX_BANK_ACCOUNTS*sizeof(bank_account_t));
   
   num_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);

   create_admin_account(argv[2]);

   server_fifo_create(&fifo_server);

   //while(1)
   server_connect_user(&fifo_server, &fifo_user);

   char test[4];
   while(!readline(fifo_server, test)) {
      sleep(1);
      printf("stuck in readline\n");
   }
   printf("%s\n", test);

   // ---- getting request from user
   tlv_request_t req;
   while(!read_request(fifo_server, &req)) {
      sleep(1);
      printf("stuck in read_request\n");
   }

   // ---- getting reply from server
   tlv_reply_t reply;
   acknowledge_request(&req, &reply);

   return 0;
}

void acknowledge_request(tlv_request_t *req, tlv_reply_t *reply) {
   // ---- reply type
   reply->type = req->type;

   // ---- reply value
   // header - account id
   reply->value.header.account_id = req->value.header.account_id;
   // header - ret_code + balance/transfer/shutdown;
   if (validate_request(req, reply))
      switch (req->type) {
         case OP_CREATE_ACCOUNT:
            create_user_account(&req->value.create, &reply->value);
            break;
         case OP_BALANCE:
            check_user_balance(&req->value.create, &reply->value);
            break;
         case OP_TRANSFER:
            create_user_transfer(&req->value.create, &reply->value);
            break;
         case OP_SHUTDOWN:
            shutdown_server(&req->value.create, &reply->value);
            break;
         default:
            printf("ERROR: Invalid Operation\n");
            break;
      }

   // ---- reply length
   reply->length = sizeof(reply->value);
}

int validate_request(tlv_request_t *req, tlv_reply_t *reply) {
   switch (req->type) {
      case OP_CREATE_ACCOUNT:
      case OP_SHUTDOWN:
         if(validate_admin(&req->value.header, &reply->value.header))
            return 1;
         break;
      case OP_BALANCE:
      case OP_TRANSFER:
         if(validate_user(&req->value.header, &reply->value.header))
            return 1;
         break;
      default:
         printf("ERROR: Invalid Operation\n");
         return RC_OTHER;
   }

   return 0;
}

void create_user_account(req_create_account_t* create, rep_value_t* rep_value) {
   printf("CREATING USER ACCOUNT...");
   // ---- checking if account credentials are valid
   // checking id
   if (!between(1, create->account_id, MAX_BANK_ACCOUNTS)) {
      printf("ERROR: Invalid ID - too small or too big\n");
      rep_value->header.ret_code = RC_OTHER;
      return;
   }
   if (create->account_id == accounts[create->account_id].account_id) {
      printf("ERROR: Invalid ID - id already in use\n");
      rep_value->header.ret_code = RC_ID_IN_USE;
      return;
   }
   // checking balance
   if (!between(MIN_BALANCE, create->balance, MAX_BALANCE)) {
      printf("ERROR: Invalid balance -  too little or too much\n");
      rep_value->header.ret_code = RC_OTHER;
      return;
   }
   // checking password
   size_t password_length = strlen(create->password);
   if (!between(MIN_PASSWORD_LEN, password_length, MAX_PASSWORD_LEN)) {
      printf("ERROR: Invalid password - too long or too short\n");
      rep_value->header.ret_code = RC_OTHER;
      return;
   }
   // TODO fazer com find
   strtok(create->password, " ");
   if (strlen(create->password) != password_length) {
      printf("ERROR: Invalid password - password contains spaces\n");
      rep_value->header.ret_code = RC_OTHER;
      return;
   }

   // ---- creating account
   accounts[create->account_id].account_id = create->account_id;
   accounts[create->account_id].balance = create->balance;
   // TODO generate random salt
   strcpy(accounts[create->account_id].salt,"something");
   // TODO generate hash
   strcpy(accounts[create->account_id].hash, strcat(create->password, accounts[create->account_id].salt));

   printf("\nUSER ACCOUNT CREATED.\n");
}

void check_user_balance(uint32_t id, rep_value_t* rep_value) {
   
}

void create_user_transfer(req_transfer_t* transfer, rep_value_t* rep_value) {

}

void shutdown_server(rep_value_t* rep_value) {

}

int validate_user(req_header_t *header, rep_header_t* rep_header) {
   if (is_admin(header->account_id)) {
      rep_header->ret_code = RC_OP_NALLOW;
      return 0;
   }
   char req_hash[HASH_LEN + 1];
   strcpy(req_hash, strcat(header->password, accounts[header->account_id].salt));
   // TODO make req_hash with sha256sum
   if (strcmp(req_hash, accounts[header->account_id].hash)) {
      rep_header->ret_code = RC_LOGIN_FAIL;
      return 0;
   }
   else {
      rep_header->ret_code = RC_OK;
      return 1;
   }
}

int validate_admin(req_header_t *header, rep_header_t* rep_header) {
   if (!is_admin(header->account_id)) {
      rep_header->ret_code = RC_OP_NALLOW;
      return 0;
   }
   char req_hash[HASH_LEN + 1];
   strcpy(req_hash, strcat(header->password, accounts[header->account_id].salt));
   // TODO make req_hash with sha256sum
   if (strcmp(req_hash, accounts[header->account_id].hash)) {
      rep_header->ret_code = RC_LOGIN_FAIL;
      return 0;
   }
   else {
      rep_header->ret_code = RC_OK;
      return 1;
   }
}

void create_admin_account(char* password) {
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

   accounts[ADMIN_ACCOUNT_ID].account_id = ADMIN_ACCOUNT_ID;
   accounts[ADMIN_ACCOUNT_ID].balance = 0;
   // TODO generate random salt
   strcpy(accounts[ADMIN_ACCOUNT_ID].salt,"something");
   // TODO generate hash
   strcpy(accounts[ADMIN_ACCOUNT_ID].hash, strcat(password, accounts[ADMIN_ACCOUNT_ID].salt));
   
   printf("\nADMIN ACCOUNT CREATED.\n");
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
   n = read(fd, req, sizeof(tlv_request_t));
   if (n > 0)
      printf("type: %d\tlenght: %d\tvalue: PID: %d\n", req->type, req->length, req->value.header.pid);

   return (n>0);
}
