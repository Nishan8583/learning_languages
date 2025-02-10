#include <stdio.h>
#include <stdlib.h>

int main() {

  // with malloc values are not zeroed out
  int *a = malloc(
      sizeof(int) *
      10); // we want to store int, its basically dynamically allocated array

  for (int i = 0; i < 10; i++) {
    printf("Default Value in position %d is %d with malloc\n", i, a[i]);
  }

  for (int i = 0; i < 10; i++) {
    a[i] = i;
  }

  for (int i = 0; i < 10; i++) {
    printf("Value in position %d is %d with malloc\n", i, a[i]);
  }

  free(a);

  int *b = calloc(
      10, sizeof(int)); // calloc zeroes it out, function signature is diffeernt
  //

  for (int i = 0; i < 10; i++) {
    printf("Default Value in position %d is %d with calloc\n", i, b[i]);
  }

  for (int i = 0; i < 10; i++) {
    b[i] = i;
  }

  for (int i = 0; i < 10; i++) {
    printf("Value in position %d is %d with calloc\n", i, b[i]);
  }

  // realloc with reallocate memroy, and coopy all the values from previous
  // location
  b = realloc(b, 15); // we want to add 5 more values

  for (int i = 0; i < 15; i++) {
    printf("Default Value in position %d is %d with realloc\n", i, b[i]);
  }

  for (int i = 0; i < 15; i++) {
    b[i] = i;
  }

  for (int i = 0; i < 15; i++) {
    printf("Value in position %d is %d with realloc\n", i, b[i]);
  }
  free(b);
}
