#include <stdio.h>

int main() {
  char name[1000];

  printf("Please enter name: ");
  fgets(name, 1000, stdin); // sfaelu get (dest pointer, limit, file pointer)
  printf("hello %s\n", name);
}
