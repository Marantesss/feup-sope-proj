#include "user.h"

int main(int argc, char *argv[]){
   tlv_request_t request;
   tlv_reply_t reply;
   int fifo_reply, fifo_request;
   char user_fifo_path[USER_FIFO_PATH_LEN];

   setbuf(stdout, NULL); // prints stuff without needing \n

   // ---- checking the minimum ammount of argumrnts
   if (argc < 6){
      printf("Error: Too few arguments\n");
      printf("Usage: %s [account-id] [\"password\"] [delay-in-ms] [operation-id] [\"[]/[acc-id amount]/[acc-id balance password]\"]\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   // ---- checking the maximum ammount of arguments
   if (argc > 6){
      printf("Error: Too many arguments\n");
      printf("Usage: %s [account-id] [\"password\"] [delay-in-ms] [operation-id] [\"[]/[acc-id amount]/[acc-id balance password]\"]\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   // ---- connect to server/request fifo
   user_connect_server(&fifo_request);

   // ---- get request
   get_request(argv, &request);

   // ---- write request
   write(fifo_request, &request, sizeof(tlv_request_t));

   // ---- close server/request fifo
   close(fifo_request);

   // ---- connect to user/reply fifo
   user_connect_fifo_reply(&fifo_reply, user_fifo_path);

   // ---- read reply
   read_reply(fifo_reply, &reply);

   // ---- print reply (Reply may not be successfull - show errors with return code)
   print_reply(&reply);

   // ---- close and delete user/reply fifo
   close(fifo_reply);
   remove(user_fifo_path);

   return 0;
}

void user_connect_server(int *fifo_request){
   printf("Opening Server/Request FIFO communication channel...");

   // ---- opening server fifo - does not wait for server
   *fifo_request = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK); // | O_NONBLOCK makes so that it does not block
   if (*fifo_request == -1){
      printf("\nfifo_server: Open attempt failed.\n");
      exit(-1);
   }

   printf("\nChannel opened.\nUser %d connected to server.\n", getpid());
}

void user_connect_fifo_reply(int *fifo_reply, char* user_fifo_path) {
   printf("Opening User/Reply FIFO communication channel...");

   // ---- opening user fifo - waits for server to create fifo and connect to user
   // get user fifo path name;
   get_user_fifo_path(user_fifo_path);
   // wait for user fifo to be created
   while (access(user_fifo_path, F_OK))
      sleep(1);
   // opening user fifo - waits for server to connect to user
   *fifo_reply = open(user_fifo_path, O_RDONLY);
   if (*fifo_reply == -1){
      printf("\nfifo_user: Open attempt failed\n");
      exit(-1);
   }

   printf("\nChannel opened.\n");
}

void get_user_fifo_path(char *user_fifo_path) {

   strcpy(user_fifo_path, USER_FIFO_PATH_PREFIX);
   char user_fifo_path_sufix[WIDTH_ID + 1];
   sprintf(user_fifo_path_sufix, "%d", getpid());
   strcat(user_fifo_path, user_fifo_path_sufix);
}

void get_request(char *argv[], tlv_request_t *request){
   int arg_receiver, id_op;
   char *token;

   printf("WARNING: No verifications are being made to command arguments!!!\n");

   // **** id da conta
   arg_receiver = atoi(argv[1]);
   request->value.header.account_id = arg_receiver;

   // **** password
   strcpy(request->value.header.password, argv[2]);

   // **** delay in ms
   arg_receiver = atoi(argv[3]);
   request->value.header.op_delay_ms = arg_receiver;

   // **** process id
   request->value.header.pid = getpid();

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

int read_reply(int fd, tlv_reply_t *reply){
   int n;

   // reads pointer
   n = read(fd, reply, sizeof(tlv_reply_t));

   return (n > 0);
}

int print_reply(tlv_reply_t *reply){

   switch (reply->value.header.ret_code){
   case 0:
      switch (reply->type){
      case 0: //create account
         printf("OP_CREATE_ACCOUNT\n");
         break;
      case 1: //ver saldo
         printf("YOUR BALANCE IS %d\n", reply->value.balance.balance);
         break;
      case 2: //transfere moneys
         printf("TRANSFER SUCCESSFUL. BALANCE IS NOW %d\n", reply->value.transfer.balance);
         break;
      case 3: //desliga o servidor acho eu
         printf("SERVER SHUTDOWN. %d ACTIVE OFFICES REMAINING\n", reply->value.shutdown.active_offices);
         break;
      default:
         break;
      }
      break;
   case 1:
      printf("The server is currently (down).\n");
      break;
   case 2:
      printf("The request sent to server timed out.\n");
      break;
   case 3:
      printf("Unable to send reply message to user.\n");
      break;
   case 4:
      printf("Invalid account id / password.\n");
      break;
   case 5:
      printf("You are not allowed to request such an operation.\n");
      break;
   case 6:
      printf("That account id is already in use.\n");
      break;
   case 7:
      printf("There is no account with that id.\n");
      break;
   case 8:
      printf("The source and destination accounts are the same.\n");
      break;
   case 9:
      printf("The final balance would be too low.\n");
      break;
   case 10:
      printf("The final balance would be too high.\n");
      break;
   case 11:
   default:
      printf("An unknown error occured\n");
      break;
   }

   return 0;
}
