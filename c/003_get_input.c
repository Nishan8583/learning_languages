#include <stdio.h>

int main() {
  char name[100];

  printf("Enter name: ");
  scanf("%[^\n]100s",
        name); // name without indexing gives address of name,  regular
               // expreession [^\n] match any char thats not a new line, or
               // until we hit 100 charactes
  printf("\n");
  printf("Hello %s\n", name);
}
