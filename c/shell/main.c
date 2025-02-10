#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // for getcwd
#define MAX_ARGS 64

void print_shell_prefix() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) { // returns NULL on error
    perror("Unable to get current working directory");
    printf(">>> ");
  }

  printf("%s >>> ", cwd);
}

void get_user_input(char *buf, size_t size) {

  memset(buf, 0, size);

  // get input from standard input
  if (fgets(buf, size, stdin) == NULL) {
    perror("Error while getting input from standard input");
    exit(-1);
  }

  buf[strcspn(buf, "\n")] = 0;

  // array of pointers, each pointer is of size char*
  char **args = malloc(MAX_ARGS * sizeof(char *));

  if (args == NULL) {
    printf("Unable to malloc for  parsing command line");
    exit(-1);
  }

  char *token = strtok(buf, " \n");

  if (token == NULL) {
    printf("Unable to tokenize");
    exit(-1);
  }

  int i = 0;
  while (token != NULL && i < MAX_ARGS - 1) {
    args[i++] = token;
    token = strtok(NULL, " \n");
  }

  if (args[0] == NULL) {
    return;
  }

  char *exit_string = "exit";
  char *cd_string = "cd";
  if (strcmp(exit_string, args[0]) == 0) {
    printf("Exiting");
    exit(0);
  } else if (strcmp(cd_string, args[0]) == 0)
    if (chdir(args[1]) != 0) {
      perror("Could not change directory");
      return;
    }
  // after we run fork(), another child process has started, now both
  // process continoue from this point not from main, so when they check
  // and csee tat the pid is 0 thats the child, and we want that child to
  // execute the command and exit
  pid_t pid = fork();

  if (pid == -1) {
    perror("failed to fork ");
    exit(-1);
  } else if (pid == 0) {
    execvp(args[0], args);
    exit(0);
  } else {
    wait(NULL); // waiting for child to finish
  }
  free(args);
}

int main() {
  char buf[1024];
  while (1) {
    print_shell_prefix();
    get_user_input(buf, 1024);
  }
}
