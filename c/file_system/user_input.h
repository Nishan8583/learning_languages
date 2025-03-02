#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef USER_INPUT
#define USER_INPUT
#define false 0
#define true 1

#define MAX_USER_INPUT 4096

#define ERR_USER_INPUT 1
#define ERR_ALLOC_FAILURE 2

int handle_user_input() {
  // intialize file system
  FILE_SYSTEM *fs;

  char FS_PATH[] = "file_systest.fs";
  fs = load_fs(FS_PATH);
  if (fs == NULL) {
    printf("handle_user_input(): existing file system not found, attempting to "
           "create new one\n");
    fs = calloc(1, sizeof(FILE_SYSTEM));
    if (fs == NULL) {
      printf(
          "handle_user_input(): new allocation of rfile system type failed\n");
      return ERR_ALLOC_FAILURE;
    }
  }

  char buf[MAX_USER_INPUT];
  char quit[] = "quit";

  memset(buf, 0, MAX_USER_INPUT);

  while (true) {

    if (fgets(buf, MAX_USER_INPUT, stdin) == NULL) {
      perror("handle_user_input(): could not read user input");
      return ERR_USER_INPUT;
    }

    buf[strcspn(buf, "\n")] =
        '\0'; // strcpn will give the index of first occurence of \n, so we are
              // basically replacing \n with \0 here, since fgets add an extra
              // \n
    if (strcmp(quit, buf) == 0) {
      printf("exiting\n");
      return 0;
    }
    printf("User input was %s\n", buf);
  }
  return 0;
}
#endif
