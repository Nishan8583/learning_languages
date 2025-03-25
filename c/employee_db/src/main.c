#include <getopt.h>  // for command line handling
#include <stdbool.h>  // for boolean easiness
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>

#include "file.h"
#include "parse.h"
#include "common.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <filename>\n",argv[0]);
  printf("\t -n is flag to signal a new file shoudl be created\n");
  printf("\t -f points the filename/path for the db\n");
  return;
}

int main(int argc, char *argv[]) {

  int c = 0;
  char *filepath = NULL;
  char *add_string = NULL;
  bool newfile= false;
  bool list_employees = false;
  int dbfd = -1;
  dbheader_t *dbhdr = NULL;
  employee_t *employees = NULL;

  // getopts returns -1 when no more parsing is left
  // flags are third arguement, n means bool
  // f is string, string is denoted by colon
  while ((c = getopt(argc,argv,"nf:a:l")) != -1) {
    switch (c) {
      case 'f':
        filepath = optarg; // optarg will hold pointer to current variable
        break;
      case 'n':
        newfile = true;
        break;
      case 'a':
        add_string = optarg;
        break;
      case 'l':
        list_employees = true;
        break;
      case '?':
        printf("Unkown arguement -%c\n",c);
        break;
      default:
        return -1;
    }

  }


  // filepath is mandatory arguement
  if (filepath == NULL) {
    print_usage(argv);
    return 1;
  }

  if (newfile) {
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
        printf("unable to create database file\n");
        return -1;
    }

    // We are passing in &dbhdr, since dbhdr is already a pointer, *
    // &dbhdr becomes pointe to pointer **
    // if the fucntion signature only took pointer instead of double pointer,
    // if it had same name callers ointer could be modified
    // but i do not think that even happen in our case
    // anyway
    if (create_db_header(dbfd,&dbhdr) == STATUS_ERROR) {
        printf("failed to create database header\n");
        return -1;
    }
  } else {
      dbfd = open_db_file(filepath);
      if (dbfd == STATUS_ERROR) {
          printf("unable to open database file\n");
          return -1;
      }

      if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
          printf("Failed to validate header\n");
          return STATUS_ERROR;
      }
  }

  if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
      printf("failed to read employees\n");
      return STATUS_ERROR;
  }

  if (add_string) {
      if (fcntl(dbfd, F_GETFL) == -1) {
          perror("Invalid FD before write");
          return STATUS_ERROR;
      }
      printf("fd=%d\n",dbfd);
      dbhdr->count++;
      employees = realloc(employees,dbhdr->count * sizeof(employee_t));
      if (employees == NULL) {
          perror("allocation failure");
          return STATUS_ERROR;
      }
      add_employee(dbhdr,employees,add_string);
  }

  if (list_employees) {
      read_employees_list(dbhdr,employees);
  }

  if (output_file(dbfd,dbhdr,employees ) != STATUS_SUCCESS) {
      printf("could not write db header");
      return STATUS_ERROR;
  }
  return 0;
}
