#include <getopt.h>  // for command line handling
#include <stdbool.h>  // for boolean easiness
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/poll.h>

#include "file.h"
#include "parse.h"
#include "common.h"
#include "srvpoll.h"

// Some global vars
int DBFD = -1;
dbheader_t *DBHDR = NULL;
employee_t * Employees = NULL;

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <filename>\n",argv[0]);
  printf("\t -n is flag to signal a new file shoudl be created\n");
  printf("\t -f points the filename/path for the db\n");
  return;
}


// handle SIGTERM so that when i close the program with ctrl+c, db header is still valid
void handle_signal(int sig) {
    if (DBFD == -1) {
        printf("invalid file pointer for saving db\n");
        exit(-1);
    }

    if (DBHDR == NULL) {
        printf("DB header is NULL\n");
        exit(-1);
    }

    if (output_file(DBFD, DBHDR, Employees) == STATUS_ERROR) {
        printf("error while writing employees db to file\n");
        exit(-1);
    }

    exit(0);
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

  // create and bind server socket
  printf("Binding server to port 5555\n");
  int server_fd = create_and_bind_socket(5555);
  if (server_fd == STATUS_ERROR) {
      printf("invalid socket for server to listen on");
      return STATUS_ERROR;
  }

  DBHDR = dbhdr;
  DBFD = dbfd;
  Employees = employees;

  struct sigaction sa;
  sa.sa_handler = handle_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);


printf("Server is now listening \n");
if (poll_and_handle_connections(server_fd) == STATUS_ERROR ){
    printf("error during poll\n");
    close(server_fd);
    return STATUS_ERROR;
}

  if (output_file(dbfd,dbhdr,employees ) != STATUS_SUCCESS) {
      printf("could not write db header");
      close(server_fd);
      return STATUS_ERROR;
  }
  close(server_fd);
  return 0;

}
