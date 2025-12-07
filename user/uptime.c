#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char *argv[]) {

 // Insert code here.
 // You can use printf as normal.
 // Terminate the process.
 //

  if (argc != 1) {
    fprintf(1, "usage: uptime");
    exit(0);
  }

  uptime();

  exit(0);
};
