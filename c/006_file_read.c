#include <stdio.h>
#include <string.h>

// Function definitions
void override_basic_file(char *file_name);
int read_from_file(char *file_name);
void small_program(char *file_name);

int main() {
  override_basic_file("test1.txt");
  read_from_file("test1.txt");
  small_program("example.txt");
}

void override_basic_file(char *file_name) {
  FILE *h_file; // handle to a file

  h_file = fopen(file_name, "w");

  fputs("abc\n", h_file);

  // another way of writing
  int data = 5;
  fprintf(h_file, "Data=%d\n", data);

  fclose(h_file);
}

int read_from_file(char *file_name) {

  FILE *h_file;

  h_file = fopen(file_name, "r");

  char content[100];

  while (fscanf(h_file, "%s", content) != EOF) {
    printf("File content %s\n", content);
  }

  return 0;
}

void small_program(char *file_name) {
  FILE *h_file; // pointer to FILE handle type

  // Open the file, fopen returns NULL on error
  h_file = fopen(file_name, "w");
  if (h_file == NULL) {
    perror("error while trying to open a file");
    return;
  }

  char buffer[256];

  while (1) { // No true or false in C, use 0 or 1;
    printf("Type something: ");

    // get input from stdin and puut in buffer, get total of 256
    if (fgets(buffer, 256, stdin) == NULL) {
      perror("error while getting input ");
      break;
    }

    // Compare the 4 characters of buffer with string exit
    if (strncmp(buffer, "exit", 4) == 0) {
      break;
    }

    if (fputs(buffer, h_file) == EOF) {
      perror("error while writing to a file");
      break;
    }
  }

  fclose(h_file);
}
