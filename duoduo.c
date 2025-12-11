#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/* Print the prompt ">>> " and read a line of characters
   from stdin. */
int getcmd(char *buf, int nbuf) {
    // printf(">>> ");
    write(2, ">>> ", 4);
    // set a counter to control loop
    int counter = 0;
    char ch;
    // use read() to read input, store it to ch, read one char each time
    int flag = read(0, &ch, 1);
    
    // enter the while loop:
    while(flag == 1){
      // check if the input is out of the range of buf
      if(counter < nbuf -1){
        // check if user press 'Enter', break the loop
        if(ch == '\n' || ch == '\r'){
          break;
        }
        buf[counter] = ch;
        counter ++;
        flag = read(0, &ch, 1);
      }
      else{
          break;
      }
    }
    // put the end sign at the end of buf
    buf[counter] = '\0';
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

  // free arguments, otherwise:>>> echo l

  for (int i = 0; i < 10; i++) {
    if (arguments[i] != 0) {
        free(arguments[i]);
        arguments[i] = 0;
    }
  }
  numargs = 0;

  /* Flags to mark word start/end */
  int ws = 1;
  // int we = 0;

  /* Flags to mark redirection direction */
  int redirection_left = 0;
  int redirection_right = 0;
  // copy it to new viriables
  int redirection_left1 = 0;
  int redirection_right1 = 0;

  /* File names supplied in the command */
  char *file_name_l = 0;
  char *file_name_r = 0;

  // int p[2];
  int pipe_cmd = 0;

  /* Flag to mark sequence command */
  int sequence_cmd = 0;

  int i = 0;
  // length represent the length of a word
  int length = 0;
  int filename_length_l = 0;
  int filename_length_r = 0;
  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    if(buf[i] == '>'){
      redirection_right = 1;
      redirection_right1 = 1;
      continue;
    }
    if(buf[i] == '<'){
      redirection_left = 1;
      redirection_left1 = 1;
      continue;
    }
    if(buf[i] == '|'){
      pipe_cmd = 1;
    }
    if(buf[i] == ';'){
      sequence_cmd = 1;
    }
    
    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */
      // check if current char is not a space or /n 
      
      if(buf[i] != ' ' && buf[i] != '\n'){
          // check ws means this is the start of the word
          if(ws){
              arguments[numargs] = malloc(nbuf); // allocate a space for storage the new word
              memset(arguments[numargs], 0, nbuf); // clear arguments[numargs]
              ws = 0; // set start of the word to false
          }
          arguments[numargs][length] = buf[i];
          length += 1;
      }
      if(length > 0 && (buf[i] == ' ' || buf[i] == '\n')){
          // in this case, we meet a space or \n and also length is greater than zero which means currently we are in a word
          arguments[numargs][length] = '\0';
          numargs ++;
          // reset ws and we and length
          ws = 1;
          // we = 0;
          length = 0;
        }
    } else {
      /* Redirection command. Capture the file names. */
      if(redirection_left1){
        if(buf[i] != ' ' && buf[i] != '\n'){
          if(file_name_l == 0){
            file_name_l = malloc(nbuf);
            memset(file_name_l, 0, nbuf);
          }
          file_name_l[filename_length_l] = buf[i];
          filename_length_l ++;
        }
        if(filename_length_l > 0 && (buf[i] == ' ' || buf[i] == '\n')){
          // in this case, we meet a space or \n and also length is greater than zero which means currently we are in a word
          file_name_l[filename_length_l] = '\0';
          // reset ws and we and length
          ws = 1;
          // we = 0;
          filename_length_l = 0;
          redirection_left1 = 0;
        }
      }
      else{
        if(redirection_right1){
          if(buf[i] != ' ' && buf[i] != '\n'){
            if(file_name_r == 0){
              file_name_r = malloc(nbuf);
              memset(file_name_r, 0, nbuf);
            }
            file_name_r[filename_length_r] = buf[i];
            filename_length_r ++;
          }
          if(filename_length_r > 0 && (buf[i] == ' ' || buf[i] == '\n')){
            // in this case, we meet a space or \n and also length is greater than zero which means currently we are in a word
            file_name_r[filename_length_r] = '\0';
            // reset ws and we and length
            ws = 1;
            // we = 0;
            filename_length_r = 0;
            redirection_right1 = 0;
          }
        }
      }
      

    }
  }
  if(length > 0){
        // if at the end of command, there is no space or \n but we still need to store the last word:
        arguments[numargs][length] = '\0';
        numargs ++;
        ws = 1;
        length = 0;
      }
  if(redirection_left1 && file_name_l !=0){
    file_name_l[filename_length_l] = '\0';
    redirection_left1 = 0;
  }
  if(redirection_right1 && file_name_r !=0){
    file_name_r[filename_length_r] = '\0';
    redirection_right1 = 0;
  }

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    sequence_cmd = 0;
    // if not a child process
    if (fork() != 0) {
      wait(0); // parent process wait
      run_command(buf + i + 1, nbuf - (i + 1), pcp);
      // Call run_command recursively
    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    int fd;
    fd = open(file_name_l, O_RDONLY);
    if (fd < 0){
      fprintf(2, "Cannot open %s\n", file_name_l);
      exit(1);
    }
    
    close(0);
    dup(fd);
    close(fd);
  }
  if (redirection_right) {
    int fd;
    fd = open(file_name_r, O_WRONLY | O_CREATE | O_TRUNC);
    if (fd < 0){
      fprintf(2, "Cannot open %s\n", file_name_r);
      exit(1);
    }
  
    close(1);
    dup(fd);
    close(fd);
  }

  /* Parsing done. Execute the command. */

  if (numargs == 0){
    exit(0);
  }

  /*
    If this command is a CD command, write the arguments to the pcp pipe
    and exit with '2' to tell the parent process about this.
  */
  if (strcmp(arguments[0], "cd") == 0) {
    if (numargs < 2) {
        fprintf(2, "cd : require argument\n");
        exit(1);
    }
    write(pcp[1], arguments[1], strlen(arguments[1]) + 1);
    exit(2);
  } else {
    /*
      Pipe command: fork twice. Execute the left hand side directly.
      Call run_command recursion for the right side of the pipe.
    */
    if (pipe_cmd) {
      exit(0);
    } else {
      exec(arguments[0], arguments);
      fprintf(2, "exec error: %s\n", arguments[0]);
      exit(1);
      // Simple command; call exec()
    }
  }
  exit(0);
}

int main(void) {

  static char buf[100];

  int pcp[2];
  pipe(pcp);

  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(fork() == 0){
      run_command(buf, sizeof(buf), pcp);
    }
    /*
      Check if run_command found this is
      a CD command and run it if required.
    */
    int child_status;
    wait(&child_status);
    if(child_status == 2){
      char path[100] = {0};
      read(pcp[0], path, sizeof(path));
      chdir(path);
    }
  }
  exit(0);
}

