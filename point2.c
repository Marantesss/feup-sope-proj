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
  char out[256];

  if (argc >= 2) {
    strcat(command, argv[1]);
  }

  FILE *f = popen(command, "r");

  while (fgets(out, 256, f) != NULL) {
    printf("%s", out);
  }

  pclose(f);

  exit(0);
}
