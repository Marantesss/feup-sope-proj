#include "user.h"

int main(int argc, char *argv[]) {
   tlv_request_t request;
   tlv_reply_t reply;
   int fifo_user, fifo_server;

   setbuf(stdout, NULL); // prints stuff without needing \n

   // ---- checking the minimum ammount of argumrnts
   if (argc < 6) {
      printf("Error: Too few arguments\n");
      printf("Usage: %s [account-id] [\"password\"] [delay-in-ms] [operation-id] [\"[]/[acc-id amount]/[acc-id balance password]\"]\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   // ---- checking the maximum ammount of arguments
   if (argc > 6) {
      printf("Error: Too many arguments\n");
      printf("Usage: %s [account-id] [\"password\"] [delay-in-ms] [operation-id] [\"[]/[acc-id amount]/[acc-id balance password]\"]\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   // ---- connect to server/request fifo
   user_connect_server(&fifo_server);

   // ---- get request
   get_request(argv, &request);

   // ---- write request
   write(fifo_server, &request, sizeof(tlv_request_t));

   // ---- connect to user/reply fifo
   user_connect_user_fifo(&fifo_user);

   // ---- read reply
   read_reply(fifo_user, &reply);
   
   return 0;
}

void user_connect_server(int* fifo_server) {
   printf("Opening Server/Request FIFO communication channel...");

   // ---- opening server fifo - does not wait for server
   *fifo_server = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_server == -1) {
      printf("\nfifo_server: Open attempt failed.\n");
      exit(-1);
   }

   printf("\nChannel opened.\nUser %d connected to server.\n", getpid());
}

void user_connect_user_fifo(int* fifo_user) {
   printf("Opening User/Reply FIFO communication channel...");

   // ---- opening user fifo - waits for server to create fifo and connect to user
   // get user fifo path name
   char user_fifo_path[USER_FIFO_PATH_LEN];
   get_user_fifo_path(user_fifo_path);
   // wait for user fifo to be created
   while(access(user_fifo_path, F_OK)) sleep(1);
   // opening user fifo - waits for server to connect to user
   *fifo_user = open(user_fifo_path, O_RDONLY);
   if (*fifo_user == -1) {
      printf("\nfifo_user: Open attempt failed\n");
      exit(-1);
   }

   printf("\nChannel opened.\n");

}

void get_user_fifo_path(char* user_fifo_path) {
   strcpy(user_fifo_path, USER_FIFO_PATH_PREFIX);
   char user_fifo_path_sufix[WIDTH_ID + 1];
   sprintf(user_fifo_path_sufix, "%d", getpid());
   strcat(user_fifo_path, user_fifo_path_sufix);
}

void get_request(char *argv[], tlv_request_t* request) {
   int arg_receiver, id_op;
   char *token;

   // **** id da conta
   arg_receiver = atoi(argv[1]);
   request->value.header.account_id = arg_receiver;

   // **** password
   strcpy(request->value.header.password, argv[2]);

   // **** delay in ms
   arg_receiver = atoi(argv[3]);
   request->value.header.op_delay_ms = arg_receiver;

   // **** id da operação e "argumentos extra" aka argv[5]
   id_op = atoi(argv[4]);
   switch (id_op){
   case 0: //create account
      request->type = OP_CREATE_ACCOUNT;

      token = strtok(argv[5], " ");
      int account_created = atoi(token);
      request->value.create.account_id = account_created;

      token = strtok(NULL, " ");
      int balance = atoi(token);
      request->value.create.balance = balance;

      token = strtok(NULL, " ");
      strcpy(request->value.create.password, token);

      break;
   case 1: //ver saldo
      request->type = OP_BALANCE; //ignora o ultimo argumento, string vazia
      break;
   case 2: //transfere moneys
      request->type = OP_TRANSFER;

      token = strtok(argv[5], " ");
      int account_destination = atoi(token);
      request->value.transfer.account_id = account_destination;

      token = strtok(NULL, " ");
      int amount = atoi(token);
      request->value.transfer.amount = amount;

      break;
   case 3: //desliga o servidor acho eu
      request->type = OP_SHUTDOWN; //ignora o ultimo argumento, string vazia
      break;
   default:
      break;
   }
}

int read_reply(int fd, tlv_reply_t* reply) {
   int n;

   // reads pointer
   n = read(fd, reply, sizeof(tlv_reply_t));

   return (n>0);
}

int readline(int fd, char *str) {
   int n;

   do {
      n = read(fd, str, 1);
   } while (n>0 && *str++ != '\0');

   return (n>0);
}

/*
void create_test_request(tlv_request_t* req) {
   // ---- type
   req.type = OP_CREATE_ACCOUNT;
   // ---- value
   // header
   req.value.header.account_id = 0;
   req.value.header.op_delay_ms = 10;
   strcpy(req.value.header.password,"olaolaol");
   req.value.header.pid = getpid();
   // create
   req.value.create.account_id = 2;
   req.value.create.balance = 100;
   strcpy(req.value.create.password, "ola");
   // ---- length
   req.length = sizeof(req.value);
}
*/
