#include "utils.h"

tlv_request_t *command = NULL;

int main(int argc, char *argv[], char* envp[]) {
   printf("USER still in development\n!");

   command = calloc(1, sizeof(tlv_request_t));

   //-------parsing arguments
   //for(int i = 1; i < argc - 1; i++){
      switch (*argv[4]){
      case '0': //create account
         command->type = OP_CREATE_ACCOUNT;
         break;
      case '1':
         command->type = OP_BALANCE;
         if()
         break;
      case '2':
         command->type = OP_TRANSFER;
         break;
      case '3':
         command->type = OP_SHUTDOWN;
         break; 
      default:
         break;
      }
   //}

   return 0;
}