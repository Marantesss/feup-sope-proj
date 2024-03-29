#include "utils.h"

char *rand_string(char *str, size_t size) {
   const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
   if (size) {
     --size;
      for (size_t n = 0; n < size; n++) {
         int key = rand() % (int) (sizeof charset - 1);
         str[n] = charset[key];
      }
      str[size] = '\0';
   }
   return str;
}
