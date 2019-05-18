#include "user.h"
#include "log.c"

int main(int argc, char *argv[]){
   tlv_request_t request;
   tlv_reply_t reply;
   int fifo_reply, fifo_request;
   int logfile;
   logfile = open(USER_LOGFILE, O_APPEND);
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
   logRequest(logfile,request.value.header.pid, &request);

   // ---- write request
   write(fifo_request, &request, sizeof(tlv_request_t));

   // ---- close server/request fifo
   close(fifo_request);

   // ---- connect to user/reply fifo
   user_connect_fifo_reply(&fifo_reply, user_fifo_path);

   // ---- read reply
   read_reply(fifo_reply, &reply);
   logReply(logfile, getpid(), &reply);

   // ---- print reply (Reply may not be successfull - show errors with return code)
   print_reply(&reply);

   // ---- close and delete user/reply fifo
   close(fifo_reply);
   remove(user_fifo_path);

   // ---- close ulog
   close(logfile);

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
   case OP_CREATE_ACCOUNT: //create account
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
   case OP_BALANCE: //ver saldo
      request->type = OP_BALANCE; //ignora o ultimo argumento, string vazia
      break;
   case OP_TRANSFER: //transfere moneys
      request->type = OP_TRANSFER;

      token = strtok(argv[5], " ");
      int account_destination = atoi(token);
      request->value.transfer.account_id = account_destination;

      token = strtok(NULL, " ");
      int amount = atoi(token);
      request->value.transfer.amount = amount;

      break;
   case OP_SHUTDOWN: //desliga o servidor acho eu
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
   case RC_OK:
      switch (reply->type){
      case OP_CREATE_ACCOUNT: //create account
         printf("OP_CREATE_ACCOUNT\n");
         break;
      case OP_BALANCE: //ver saldo
         printf("YOUR BALANCE IS %d\n", reply->value.balance.balance);
         break;
      case OP_TRANSFER: //transfere moneys
         printf("TRANSFER SUCCESSFUL. BALANCE IS NOW %d\n", reply->value.transfer.balance);
         break;
      case OP_SHUTDOWN: //desliga o servidor acho eu
         printf("SERVER SHUTDOWN. %d ACTIVE OFFICES REMAINING\n", reply->value.shutdown.active_offices);
         break;
      default:
         break;
      }
      break;
   case RC_SRV_DOWN:
      printf("The server is currently (down).\n");
      break;
   case RC_SRV_TIMEOUT:
      printf("The request sent to server timed out.\n");
      break;
   case RC_USR_DOWN:
      printf("Unable to send reply message to user.\n");
      break;
   case RC_LOGIN_FAIL:
      printf("Invalid account id / password.\n");
      break;
   case RC_OP_NALLOW:
      printf("You are not allowed to request such an operation.\n");
      break;
   case RC_ID_IN_USE:
      printf("That account id is already in use.\n");
      break;
   case RC_ID_NOT_FOUND:
      printf("There is no account with that id.\n");
      break;
   case RC_SAME_ID:
      printf("The source and destination accounts are the same.\n");
      break;
   case RC_NO_FUNDS:
      printf("The final balance would be too low.\n");
      break;
   case RC_TOO_HIGH:
      printf("The final balance would be too high.\n");
      break;
   case RC_OTHER:
   default:
      printf("An unknown error occured\n");
      break;
   }

   return 0;
}

int readline(int fd, char *str){
   int n;

   do{
      n = read(fd, str, 1);
   } while (n > 0 && *str++ != '\0');

   return (n > 0);
}
