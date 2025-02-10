#include <stdio.h>

int main() {
  char line[1000];
  FILE *hand; // FILE is a type defined in stdio, so hand is a pointer to FILE

  hand = fopen("romeo.txt", "r");

  // fgets keeps reading 1000 line, we get EOF, i.e. NULL when we reach end
  while (fgets(line, 1000, hand) != NULL) {
    printf("%s", line);
  }
}
