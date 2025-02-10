#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *password1;
  char *password2;
  char *stale_ptr;

  password1 = malloc(sizeof(char) * 10);
  password2 = malloc(sizeof(char) * 10);

  printf("p1=%p p2=%p\n", password1, password2);
  printf("In int p1=%d,p2=%d\n", (int)password1, (int)password2);

  strcpy(password1, "lol");
  strcpy(password2, "pass");

  stale_ptr = password1;
  password1 = realloc(password1, 20);
  printf("Old pointer still has old password %s\n", stale_ptr);
}
