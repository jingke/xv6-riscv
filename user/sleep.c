#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char *argv[]) {

 // Insert code here.
 // You can use printf as normal.
 // Terminate the process.
 //
  int i = 1;

  if (argc != 2) {
    fprintf(1, "usage: sleep <ticks>");
    exit(0);
  }

  char* command = argv[0];

  if (strcmp(command, "sleep") != 0) {
    fprintf(1, "command is not sleep!");
    exit(1);
  }

  char* seconds = argv[i];
  int sec = atoi(seconds);

  if (sec == 0) {
    fprintf(1, "ticks must be integer");
    exit(1);
  }

   int pid = fork();
 if(pid > 0){
  // parent process
 printf("parent process: child=%d\n", pid);
 pid = wait((int *) 0);
 printf("child %d is done\n", pid);
 } else if(pid == 0){
 pause (100);
// child process 
 printf("child process: exiting\n");
 exit(0);
 } else {
 printf("fork error\n");
 }

  pause(sec);

  exit(0);
};
