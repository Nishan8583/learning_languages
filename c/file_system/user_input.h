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
  char ls[] = "ls";
  char create[] = "create";
  char insert[] = "insert";
  char read_str[] = "read";

  memset(buf, 0, MAX_USER_INPUT);

  while (true) {
    printf(">>> ");
    if (fgets(buf, MAX_USER_INPUT, stdin) == NULL) {
      perror("handle_user_input(): could not read user input");
      return ERR_USER_INPUT;
    }

    buf[strcspn(buf, "\n")] =
        '\0'; // strcpn will give the index of first occurence of \n, so we are
              // basically replacing \n with \0 here, since fgets add an extra
              // \n
    if (strcmp(quit, buf) == 0) {
      printf("saving file system");
      if (save_fs(fs, FS_PATH) != 0) {
        printf("Error while saving file\n");
      }
      printf("exiting\n");
      return 0;

    } else if (strcmp(create, buf) == 0) {
      char filename[256];
      printf("Please enter the filename >> ");
      if (fgets(filename, 256, stdin) == NULL) {
        printf("could not read filename \n");
        return ERR_USER_INPUT;
      }
      if (create_file(fs, filename) != SUCCESS_CREATE_FILE) {
        printf("error while creating file\n");
        return ERR_USER_INPUT;
      }

    } else if (strcmp(ls, buf) == 0) {
      list_files(fs);

    } else if (strcmp(insert, buf) == 0) {
      char dest_file[256];
      char content[4096];
      printf("please enter the filename u want to insert the text for: >>> ");
      fgets(dest_file, 256, stdin);
      printf("please enter the text >>> ");
      fgets(content, 4096, stdin);
      if (file_exists(fs, dest_file)) {
        printf("file exists\n");
        if (write_to_file(fs, dest_file, content) == WRITE_FILE_FAILURE) {
          printf("unable to write to a file\n");
        }
      } else {
        printf("file does not exist\n");
      }

    } else if (strcmp(read_str, buf) == 0) {
      char src_file[256];
      printf("which file do you want to read >>> ");
      fgets(src_file, 256, stdin);
      char *content = read_from_file(fs, src_file);
      if (content == NULL) {
        printf("could not read from file\n");
      } else {
        printf("File contents: %s", content);
      }

    } else {
      printf("Unknown command\n");
    }
  }
  return 0;
}
#endif
