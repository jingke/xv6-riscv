#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/* Define whitespace characters for parsing, similar to sh.c */
char whitespace[] = " \t\r\n\v";

/* Print the prompt ">>> " and read a line of characters
   from stdin. */   
int getcmd(char *buf, int nbuf) {
  // Print prompt to standard error
  write(2, ">>> ", 4);
  
  // Clear buffer and read input
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  
  // Return -1 if input is empty (EOF)
  if(buf[0] == 0) {
    return -1;
  }
  
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

  /* Parse the command character by character. */
  for (; i < nbuf; i++) {
    /* Skip null terminator */
    if (buf[i] == 0) {
      break;
    }

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */
    if (buf[i] == ';') {
      sequence_cmd = 1;
      buf[i] = 0;
      break;
    }
    
    if (buf[i] == '|') {
      pipe_cmd = 1;
      buf[i] = 0;
      break;
    }
    
    if (buf[i] == '<') {
      redirection_left = 1;
      buf[i] = 0;
      /* Capture input file name */
      i++;
      /* Skip whitespace */
      while (i < nbuf && strchr(whitespace, buf[i])) {
        i++;
      }
      file_name_l = &buf[i];
      /* Skip until next whitespace or end */
      while (i < nbuf && !strchr(whitespace, buf[i]) && buf[i] != '|' && buf[i] != ';' && buf[i] != 0) {
        i++;
      }
      if (i < nbuf) {
        buf[i] = 0;
      }
      continue;
    }
    
    if (buf[i] == '>') {
      redirection_right = 1;
      buf[i] = 0;
      /* Capture output file name */
      i++;
      /* Skip whitespace */
      while (i < nbuf && strchr(whitespace, buf[i])) {
        i++;
      }
      file_name_r = &buf[i];
      /* Skip until next whitespace or end */
      while (i < nbuf && !strchr(whitespace, buf[i]) && buf[i] != '|' && buf[i] != ';' && buf[i] != 0) {
        i++;
      }
      if (i < nbuf) {
        buf[i] = 0;
      }
      continue;
    }

    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */
      /* Parse command arguments */
      if (strchr(whitespace, buf[i])) {
        /* End of current argument */
        if (ws == 0) {
          we = 1;
          ws = 1;
          buf[i] = 0;
        }
      } else {
        /* Start of new argument */
        if (ws == 1) {
          arguments[numargs++] = &buf[i];
          ws = 0;
          we = 0;
        }
      }
    } else {
      /* Redirection command. Capture the file names. */
      /* Already handled above */
    }
  }
  
  /* Null-terminate the arguments array */
  arguments[numargs] = 0;

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    sequence_cmd = 0;
    if (fork() != 0) {
      wait(0);
      /* Call run_command recursively on the remaining command */
      run_command(&buf[i+1], nbuf - (i+1), pcp);
    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    int fd = open(file_name_l, O_RDONLY);
    if (fd < 0) {
      fprintf(2, "Error opening input file: %s\n", file_name_l);
      exit(1);
    }
    close(0);
    dup(fd);
    close(fd);
  }
  if (redirection_right) {
    int fd = open(file_name_r, O_WRONLY | O_CREATE);
    if (fd < 0) {
      fprintf(2, "Error opening output file: %s\n", file_name_r);
      exit(1);
    }
    close(1);
    dup(fd);
    close(fd);
  }

  /* Parsing done. Execute the command. */

  /* Handle pipe command (|) */
  if (pipe_cmd) {
    if (pipe(p) < 0) {
      fprintf(2, "Error creating pipe\n");
      exit(1);
    }
    
    if (fork() != 0) {
      /* Parent process - execute the right side of the pipe */
      close(p[1]);
      close(0);
      dup(p[0]);
      close(p[0]);
      run_command(&buf[i+1], nbuf - (i+1), pcp);
    } else {
      /* Child process - execute the left side of the pipe */
      close(p[0]);
      close(1);
      dup(p[1]);
      close(p[1]);
    }
  }
  
  /* Execute the command */
  if (numargs == 0) {
    /* No command to execute */
    exit(0);
  }
  
  /* Handle built-in commands */
  if (strcmp(arguments[0], "exit") == 0) {
    /* Write exit signal to pipe and exit */
    write(pcp[1], "exit", 4);
    write(pcp[1], "\0", 1);
    exit(0);
  }
  
  /* Execute external commands */
  exec(arguments[0], arguments);
  
  /* If exec returns, it failed */
  fprintf(2, "Error executing command: %s\n", arguments[0]);
  exit(1);
  
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