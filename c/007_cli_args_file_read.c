#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    perror("Please provide a file to read");
    return -1;
  }

  FILE *file;
  char *file_name = argv[1];

  file = fopen(file_name, "r");
  if (file == NULL) {
    perror("ERROR could not read file");
    return -1;
  }

  char content[256];
  while (fgets(content, 256, file) != NULL) {
    printf("%s", content);
  }
  fclose(file);
}
