#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/* Print the prompt ">>> " and read a line of characters
   from stdin. */   
int getcmd(char *buf, int nbuf) {

  // ##### Place your code here

  // Use the 'ws' variable to suppress the 'unused variable' warning for now.
  // Will be used later for parsing.
  int ws_dummy = 0; 
  ws_dummy++; // Prevent unused variable warning
	      //
	      //
  write(2, ">>> ", 4);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

/*
  A recursive function which parses the command
  at *buf and executes it.
*/
__attribute__((noreturn))
void run_command(char *buf, int nbuf, int *pcp) {

  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 1;
  int we = 0;

  /* Flags to mark redirection direction */
  int redirection_left = 0;
  int redirection_right = 0;

  /* File names supplied in the command */
  char *file_name_l = 0;
  char *file_name_r = 0;

  int p[2];
  int pipe_cmd = 0;

  /* Flag to mark sequence command */
  int sequence_cmd = 0;

  int i = 0;

  // Suppress 'unused variable' warnings for variables we aren't using yet, 
  // but must keep due to template constraints.
  (void)p;
  (void)numargs;
  (void)ws;
  (void)we;
  (void)file_name_l;
  (void)file_name_r;
  (void)pipe_cmd;
  (void)arguments;

  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    /* ##### Place your code here. */

    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */

      // Place your code here.
    } else {
      /* Redirection command. Capture the file names. */

      // ##### Place your code here.
    }
  }

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    sequence_cmd = 0;
    if (fork() != 0) {
      wait(0);
      // ##### Place your code here.
      // Call run_command recursively
    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    // ##### Place your code here.
  }
  if (redirection_right) {
    // ##### Place your code here.
  }

  /* Parsing done. Execute the command. */

  /*
    WARNING FIX: We must avoid using the uninitialized 'arguments' array
    before we write the parsing code. We temporarily exit here 
    to prevent the uninitialized variable error (E.g., for 'cd' check).
    The actual CD and exec logic must replace this temporary code.
  */
  
  // If the command is 'cd', the logic should be handled by the parent 'main' process.
  // We use exit(1) for all unhandled cases for now.
  exit(1);

  /*
    // Original template structure that caused UNINITIALIZED error:
    if (strcmp(arguments[0], "cd") == 0) {
      // ##### Place your code here.
    } else {
      // ##### Place your code here.
    }
  */
  
}

int main(void) {

  static char buf[100];

  int pcp[2];
  pipe(pcp);

  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(fork() == 0)
      run_command(buf, 100, pcp);

    /*
      Check if run_command found this is
      a CD command and run it if required.
    */
    // We must ensure the 'child_status' variable is used or removed.
    // Since it's needed later for 'wait', we initialize it to suppress the warning.
    int child_status = 0; 
    
    // ##### Place your code here
    
    // To prevent unused variable warning, we call wait and use the status.
    wait(&child_status); 

    // The actual CD logic will go here later.
  }
  exit(0);
}
