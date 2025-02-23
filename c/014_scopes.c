#include <stdio.h>

// Global scope | File scope
int z = 3; // scope on entire file
//
// include
// Good practice to avoide global variables
// side effects, a lot of places where it can be modified
int x = 10;
int main() {

  // local variables takes precedence over same name
  // Try to avoid this
  int x = 5; // scope inside main not anywhere else
  {
    // block scope
    int y = 10;
    printf("y: %d\n", y);
  }
  printf("x: %d\n", x);
  // printf("y: %d\n", y);  //not in scope
  printf("z: %d\n", z);
  return 0;
}
