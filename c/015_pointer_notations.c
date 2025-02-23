#include <stdio.h>

void print(int *a); // allows to pass arrays or a pointe to an array
int main() {

  // These were array notation
  // More intuitive
  // Arrays can be treated as pointers and vice versa
  int a[3] = {1, 2, 3};
  int *b;

  b = a; // arrays decays to a pointer
  //
  printf("b: %p and a=%p\n", b,
         a); // a really holds pointer to memory, and now b also stores the same
  //
  print(a); // a decays into a pointer
  //
  print(a + 1); // next elment
  // include

  // Pointer notation
  printf("*b = %d\n", *b); // go get the value b points to
  printf("*a = %d\n", *a); // go get the value b points to
  // include

  // pointer arithmetic
  // Get the pointer +1 value that b points to, here address at b + size of int
  printf("*(b+1) = %d\n", *(b + 1)); // go get the value b points to
  printf("*(b+2) = %d\n", *(b + 2));

  b++; // incrememnt the epointer, point to the next elelemt, next int
  //
  // Difference, u can not assign to array

  int c[3] = {1, 2, 3};
  //  a = c; // does not compile
}

void print(int *a) {
  printf("a[1] = %d\n", a[1]); // a[1] starting fromt the memory address at a ,
                               // get a after the first one
}
