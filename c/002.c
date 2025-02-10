#include <stdio.h>

int main() {
  int usf, euf;
  printf("Default number: %d\n", usf);
  printf("Enter US floor: ");
  scanf("%d", &usf);
  printf("\n");
  euf = usf - 1;
  printf("EU Floor %d\n", euf);
}
