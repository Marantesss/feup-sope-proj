#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // **********TO DELETE**********
  //int out = open("output.txt", O_RDWR|O_CREAT|O_APPEND, 0600);
  //int pid = fork();

  // execlp("ls", "ls", "-laR", argv[1], NULL);
  // execl("/bin/ls", "ls", "-laR", argv[1], NULL);
  // execvp("ls", arg);
  // execve("/bin/ls", arg, &envp[1]);

  // char *arg[] = {"ls", "-laR", argv[1], NULL};
  // if(pid > 0){
  //     dup2((int) out, fileno(stdout));
  //     wait(NULL);
  // }
  // else{
  //     execv("/bin/ls", arg);

  //     printf("Command not executed !\n");
  //     exit(1);
  // }

  // if (pid > 0) {
  //  wait(&status);
  //  printf("I'm the parent (PID=%d)\n\n",getpid()); }
  //  else {
  //  printf("I'm the son (PID=%d)\n\n",getpid());
  //  execvp("ls",arg);
  //  printf("EXEC failed\n");
  //  }
  // *****************************

  char command[256] = "ls -la ";
  char out[10000];

  if (argc >= 2) {
    strcat(command, argv[1]);
  }

  FILE *f = popen(command, "r");

  while (fgets(out, 10000, f) != NULL) {
    // printf("%s", out);
  }

  pclose(f);

  char perms[256];
  char f_owner[256];
  char f_group[256];
  char f_size[256];
  char mod_time[3][256];
  char f_name[256];

  strcpy(perms, strtok(out, " "));

  strtok(NULL, " ");

  strcpy(f_owner, strtok(NULL, " "));

  strcpy(f_group, strtok(NULL, " "));

  strcpy(f_size, strtok(NULL, " "));

  strcpy(mod_time[0], strtok(NULL, " "));
  strcpy(mod_time[1], strtok(NULL, " "));
  strcpy(mod_time[2], strtok(NULL, " "));

  strcpy(f_name, strtok(NULL, " "));

  printf("Permissions: %s\n", perms);
  printf("File owner: %s\n", f_owner);
  printf("File group: %s\n", f_group);
  printf("File size: %s\n", f_size);
  printf("Modification time: %s %s %s\n", mod_time[0], mod_time[1], mod_time[2]);
  printf("File name: %s", f_name);

  exit(0);
}
