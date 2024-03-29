#include "server.h"

int main(int argc, char *argv[]) {
   int fifo_request;

   logfile = open(SERVER_LOGFILE, O_CREAT | O_APPEND | O_RDWR);
   if (logfile == -1) {
      printf("ERROR: Could not create and open logfile\n");
      exit(EXIT_FAILURE);
   }

   setbuf(stdout, NULL); // prints stuff without needing \n
   
   if(argc != 3) {
      printf("Too few arguments\n");
      exit(EXIT_FAILURE);
   }
   
   num_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);

   // ---- init request queue
   init(&request_queue);

   // ---- create admin account
   create_admin_account(argv[2]);
   logAccountCreation(logfile, MAIN_THREAD_ID, &accounts[ADMIN_ACCOUNT_ID]);

   // ---- create server/request fifo
   server_fifo_create(&fifo_request);

   // ---- send threads to work
   for (int i = 0; i < num_threads; i++) {
      thread_arg[i] = i + 1;
      pthread_create(&thread_id[i], NULL, thread_work, (void *)&thread_arg[i]);
   }
   
   printf("\nWaiting for user request...");

   while (1) {
      // ---- getting request from user
      tlv_request_t req;
      while(!read_request(fifo_request, &req));

      // ---- main thread should take care of shutdown requests
      if (req.type == OP_SHUTDOWN) {
         tlv_reply_t shutdown_reply;
         int fifo_reply;
         if (validate_request(&req, &shutdown_reply)) {
            shutdown_req_pid = req.value.header.account_id;
            // ---- Log the delay introduced (server shutdown only)
            logDelay(logfile, MAIN_THREAD_ID, req.value.header.op_delay_ms);
            // ---- command
            shutdown_server(&shutdown_reply.value, &fifo_request);
            // ---- send signal to unlock all threads waiting for new requests
            pthread_cond_broadcast(&cond_queued_req);
            logSyncMech(logfile, MAIN_THREAD_ID, SYNC_OP_COND_SIGNAL, SYNC_ROLE_PRODUCER, shutdown_req_pid);
            // ---- reply type
            shutdown_reply.type = req.type;
            // ---- reply length
            shutdown_reply.length = sizeof(shutdown_reply.value);
            // ---- create user fifo
            user_fifo_create(&fifo_reply, req.value.header.pid);
            // ---- write reply
            write(fifo_reply, &shutdown_reply, sizeof(tlv_reply_t));
            // ---- break cycle
            break;
         }
         // ---- reply type
         shutdown_reply.type = req.type;
         // ---- reply length
         shutdown_reply.length = sizeof(shutdown_reply.value);
         // ---- create user fifo
         user_fifo_create(&fifo_reply, req.value.header.pid);
         // ---- write reply
         write(fifo_reply, &shutdown_reply, sizeof(tlv_reply_t));
      }
      else {
         logSyncMech(logfile, MAIN_THREAD_ID, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_PRODUCER, req.value.header.account_id);
         pthread_mutex_lock(&queue_mut);
         // ---- enqueue the request
         push(&request_queue, req);
         // ---- send queued_req signal
         pthread_cond_signal(&cond_queued_req);
         logSyncMech(logfile, MAIN_THREAD_ID, SYNC_OP_COND_SIGNAL, SYNC_ROLE_PRODUCER, req.value.header.account_id);

         pthread_mutex_unlock(&queue_mut);
         logSyncMech(logfile, MAIN_THREAD_ID, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_PRODUCER, req.value.header.account_id);
      }
   }
   
   printf("\nWaiting for offices to close!\n");
   // ---- wait for threads to finish working
   for (int i = 0; i < num_threads; i++) {
      pthread_join(thread_id[i], NULL);
   }
   printf("\nOffices closed!\n");
   
   // ---- close and delete server/request fifo
   close(fifo_request);
   remove(SERVER_FIFO_PATH);

   // ---- close slog
   close(logfile);

   return 0;
}

void* thread_work(void* office_id) {
   int fifo_reply;
   tlv_reply_t reply;
   tlv_request_t next_request;

   logBankOfficeOpen(logfile, *(int *) thread_id, pthread_self());

   // ---- threads are always cheeking if there are requests available
   while (1) {
      // ---- lock threads if not shutdown
      if (!shutdown) {
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_CONSUMER, getpid());
         pthread_mutex_lock(&queue_mut);
      }
      else {
         pthread_mutex_unlock(&queue_mut);
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_CONSUMER, shutdown_req_pid);
      }

      // ---- if request queue is empty and shutodwn is set, break the loop and close offices
      if (empty(&request_queue)) {
         if (shutdown)
            break;
         else {
            logSyncMech(logfile, *(int *) thread_id, SYNC_OP_COND_WAIT, SYNC_ROLE_CONSUMER, getpid());
            pthread_cond_wait(&cond_queued_req, &queue_mut);
         }
      }
      

      if (!empty(&request_queue)) {
         // ---- get next request
         next_request = front(&request_queue);
         
         // ---- Log the delay introduced immediately after entering the critical section of an account
         logSyncDelay(logfile, *(int *) thread_id, next_request.value.header.account_id, next_request.value.header.op_delay_ms);

         // ---- increment active threads
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_CONSUMER, next_request.value.header.account_id);
         pthread_mutex_lock(&counter_mut);
         num_active_threads++;
         pthread_mutex_unlock(&counter_mut);
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_CONSUMER, next_request.value.header.account_id);

         // ---- dequeue the request
         pop(&request_queue);

         // ---- unlock threads (queue access is done)
         pthread_mutex_unlock(&queue_mut);

         // ---- process request
         acknowledge_request(&next_request, &reply, *(int *) office_id);

         // ---- create user fifo
         user_fifo_create(&fifo_reply, next_request.value.header.pid);

         // ---- write reply
         write(fifo_reply, &reply, sizeof(tlv_reply_t));

         // ---- close user fifo
         close(fifo_reply);

         // ---- decrement active threads
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_CONSUMER, next_request.value.header.account_id);
         pthread_mutex_lock(&counter_mut);
         num_active_threads--;
         pthread_mutex_unlock(&counter_mut);
         logSyncMech(logfile, *(int *) thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_CONSUMER, next_request.value.header.account_id);

         printf("Waiting for user request...");
      }
   }

   logBankOfficeClose(logfile, *(int *) office_id, pthread_self());

   return NULL;
}

void acknowledge_request(tlv_request_t *req, tlv_reply_t *reply, int office_id) {
   // ---- wait for request's delay time
   usleep(req->value.header.op_delay_ms);
   // ---- reply type
   reply->type = req->type;

   // ---- reply value
   // header - account id
   reply->value.header.account_id = req->value.header.account_id;
   // header - ret_code + balance/transfer/shutdown;
   if (validate_request(req, reply))
      switch (req->type) {
         case OP_CREATE_ACCOUNT:
            if (create_user_account(&req->value.create, &reply->value))
               logAccountCreation(logfile, office_id, &accounts[req->value.create.account_id]);
            break;
         case OP_BALANCE:
            check_user_balance(req->value.header.account_id, &reply->value);
            break;
         case OP_TRANSFER:
            create_user_transfer(req->value.header.account_id, &req->value.transfer, &reply->value);
            break;
         case OP_SHUTDOWN:
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
         return 0;
   }

   return 0;
}

int create_user_account(req_create_account_t* create, rep_value_t* rep_value) {
   printf("\nCREATING USER ACCOUNT...");
   // ---- checking if account credentials are valid
   // checking id
   if (!between(1, create->account_id, MAX_BANK_ACCOUNTS)) {
      printf("\nERROR: Invalid ID - too small or too big");
      rep_value->header.ret_code = RC_OTHER;
      return 1;
   }
   if (create->account_id == accounts[create->account_id].account_id) {
      printf("\nERROR: Invalid ID - id already in use");
      rep_value->header.ret_code = RC_ID_IN_USE;
      return 1;
   }
   // checking balance
   if (!between(MIN_BALANCE, create->balance, MAX_BALANCE)) {
      printf("\nERROR: Invalid balance -  too little or too much");
      rep_value->header.ret_code = RC_OTHER;
      return 1;
   }
   // checking password
   size_t password_length = strlen(create->password);
   if (!between(MIN_PASSWORD_LEN, password_length, MAX_PASSWORD_LEN)) {
      printf("\nERROR: Invalid password - too long or too short");
      rep_value->header.ret_code = RC_OTHER;
      return 1;
   }
   // checking for spaces in password
   if (strchr(create->password, ' ') != NULL) {
      printf("\nERROR: Invalid password - password contains spaces");
      rep_value->header.ret_code = RC_OTHER;
      return 1;
   }

   // ---- creating account
   accounts[create->account_id].account_id = create->account_id;
   accounts[create->account_id].balance = create->balance;
   // generate random salt
   char salt[SALT_LEN + 1];
   
   rand_string(salt, SALT_LEN);
   strcpy(accounts[create->account_id].salt, salt);
   // generate hash
   char command[MAX_PASSWORD_LEN + SALT_LEN + 24] = "echo -n ";

   strcat(command, create->password);
   strcat(command, salt);
   strcat(command, " | sha256sum");

   FILE *f = popen(command, "r");

   char hash[HASH_LEN + 4];

   while (fgets(hash, 10000, f) != NULL) {
      // printf("%s", out);
   }
   
   pclose(f);

   strtok(hash, " ");

   strcpy(accounts[create->account_id].hash, hash);

   rep_value->header.ret_code = RC_OK;

   printf("\nUSER ACCOUNT CREATED.\n");

   return 0;
}

void check_user_balance(uint32_t id, rep_value_t* rep_value) {
   printf("\nCHECKING USER BALANCE...");

   rep_value->transfer.balance = accounts[id].balance;
   rep_value->header.ret_code = RC_OK;

   printf("\nUSER BALANCE CHECKED - %d\n", accounts[id].balance);
}

void create_user_transfer(uint32_t id, req_transfer_t* transfer, rep_value_t* rep_value) {
   printf("\nCREATING USER TRANSFER...");
   rep_value->transfer.balance = accounts[transfer->account_id].balance; 
   // ---- checking if account credentials are valid
   // checking id
   if (!between(1, transfer->account_id, MAX_BANK_ACCOUNTS)) {
      printf("\nERROR: Invalid ID - too small or too big");
      rep_value->header.ret_code = RC_OTHER;
      return;
   }
   if (transfer->account_id != accounts[transfer->account_id].account_id) {
      printf("\nERROR: Invalid ID - id does not exist");
      rep_value->header.ret_code = RC_ID_IN_USE;
      return;
   }
   // checking funds
   if (!between(MIN_BALANCE, transfer->amount, accounts[id].balance)) {
      printf("\nERROR: Invalid transfer amount - not enough funds from sender");
      rep_value->header.ret_code = RC_NO_FUNDS;
      return;
   }
   if (!between(MIN_BALANCE, transfer->amount + accounts[transfer->account_id].balance, MAX_BALANCE)) {
      printf("\nERROR: Invalid transfer amount - too much funds from receiver");
      rep_value->header.ret_code = RC_NO_FUNDS;
      return;
   }

   accounts[id].balance -= transfer->amount;
   accounts[transfer->account_id].balance += transfer->amount;
   rep_value->header.ret_code = RC_OK;
   rep_value->transfer.balance = accounts[id].balance;

   printf("\nUSER TRANSFER CREATED.\n");
}

void shutdown_server(rep_value_t* rep_value, int *fifo_request) {
   fchmod(*fifo_request, 0444);
   shutdown = 1;
   // TODO, get number of active threads (not sure if this works)
   rep_value->shutdown.active_offices = num_active_threads;
}


int validate_user(req_header_t *req_header, rep_header_t* reply_header) {
   // ---- check id
   // check if not admin (id != 0)
   if (is_admin(req_header->account_id)) {
      printf("ERROR: Invalid ID - Client only operation\n");
      reply_header->ret_code = RC_OP_NALLOW;
      return 0;
   }
   // check if id is between 1 and 4096
   if (!between(1, req_header->account_id, MAX_BANK_ACCOUNTS)) {
      printf("ERROR: Invalid ID - too small or too big\n");
      reply_header->ret_code = RC_OTHER;
      return 0;
   }
   // check if id in use
   if (req_header->account_id != accounts[req_header->account_id].account_id) {
      printf("ERROR: Invalid ID - id does not exist\n");
      reply_header->ret_code = RC_ID_NOT_FOUND;
      return 0;
   }
   // ---- check password
   char req_hash[MAX_PASSWORD_LEN + SALT_LEN + 1];
   char command[MAX_PASSWORD_LEN + SALT_LEN + 24] = "echo -n \"";

   strcat(command, req_header->password);
   strcat(command, accounts[req_header->account_id].salt);
   strcat(command, "\" | sha256sum");

   FILE *f = popen(command, "r");

   char hash[HASH_LEN + 4];

   while (fgets(hash, 10000, f) != NULL) {
      // printf("%s", out);
   }
   
   pclose(f);

   strcpy(req_hash, strtok(hash, " "));

   if (strcmp(req_hash, accounts[req_header->account_id].hash)) {
      reply_header->ret_code = RC_LOGIN_FAIL;
      printf("ERROR: Invalid Password\n");
      return 0;
   }
   else {
      reply_header->ret_code = RC_OK;
      return 1;
   }
}

int validate_admin(req_header_t *req_header, rep_header_t* reply_header) {
   // ---- check if admin (id = 0)
   if (!(is_admin(req_header->account_id))) {
      printf("ERROR: Invalid ID - Admin only operation\n");
      reply_header->ret_code = RC_OP_NALLOW;
      return 0;
   }
   // ---- check password
   char req_hash[MAX_PASSWORD_LEN + SALT_LEN + 1];
   //strcpy(req_hash, strcat(req_header->password, accounts[req_header->account_id].salt));
   char command[MAX_PASSWORD_LEN + SALT_LEN + 24] = "echo -n \"";

   strcat(command, req_header->password);
   strcat(command, accounts[req_header->account_id].salt);
   strcat(command, "\" | sha256sum");

   FILE *f = popen(command, "r");

   char hash[HASH_LEN + 4];

   while (fgets(hash, 10000, f) != NULL) {
      // printf("%s", out);
   }
   
   pclose(f);

   strcpy(req_hash, strtok(hash, " "));
   
   if (strcmp(req_hash, accounts[req_header->account_id].hash)) {
      printf("ERROR: Invalid Password\n");
      reply_header->ret_code = RC_LOGIN_FAIL;
      return 0;
   }
   else {
      reply_header->ret_code = RC_OK;
      return 1;
   }
}

void create_admin_account(char* password) {
   printf("CREATING ADMIN ACCOUNT...");
   // ---- checking if account credentials are valid
   // checking password
   size_t password_length = strlen(password);
   if (!between(MIN_PASSWORD_LEN, password_length, MAX_PASSWORD_LEN)) {
      printf("\nERROR: Invalid password");
      exit(EXIT_FAILURE);
   }
   strtok(password, " ");
   if (strlen(password) != password_length) {
      printf("\nERROR: Invalid password");
      exit(EXIT_FAILURE);
   }

   accounts[ADMIN_ACCOUNT_ID].account_id = ADMIN_ACCOUNT_ID;
   accounts[ADMIN_ACCOUNT_ID].balance = 0;
   // TODO generate random salt
   char salt[SALT_LEN + 1];
   
   rand_string(salt, SALT_LEN);
   strcpy(accounts[ADMIN_ACCOUNT_ID].salt, salt);
   // TODO generate hash
   char command[MAX_PASSWORD_LEN + SALT_LEN + 24] = "echo -n \"";

   strcat(command, password);
   strcat(command, salt);
   strcat(command, "\" | sha256sum");

   FILE *f = popen(command, "r");

   char hash[HASH_LEN + 4];

   while (fgets(hash, 10000, f) != NULL) {
      // printf("%s", out);
   }
   
   pclose(f);

   strcpy(accounts[ADMIN_ACCOUNT_ID].hash, strtok(hash, " "));
   
   printf("\nADMIN ACCOUNT CREATED.\n");
}

void server_fifo_create(int* fifo_request) {
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
   *fifo_request = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_request == -1) {
      printf("\nfifo_server: Open attempt failed.\n");
      exit(-1);
   }

   printf("\nChannel created and connected.\nServer is online.\n");
}

void user_fifo_create(int* fifo_reply, pid_t pid) {
   printf("\nCreating user FIFO communication channels...");

   // ---- get user fifo path
   char user_fifo_path[USER_FIFO_PATH_LEN];
   strcpy(user_fifo_path, USER_FIFO_PATH_PREFIX);
   char user_fifo_path_sufix[WIDTH_ID + 1];
   sprintf(user_fifo_path_sufix, "%d", pid);
   strcat(user_fifo_path, user_fifo_path_sufix);

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

   // ---- opening user fifo - waits for user to connect to server
   *fifo_reply = open(user_fifo_path, O_WRONLY);
   if (*fifo_reply == -1) {
      printf("\nfifo_user: Open attempt failed\n");
      exit(-2);
   }

   printf("\n%s channel created.\n", user_fifo_path);
}

int read_request(int fd, tlv_request_t* req) {
   int n;

   // reads pointer
   n = read(fd, req, sizeof(tlv_request_t));
   
   return (n>0);
}
