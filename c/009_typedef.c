#include <stdio.h>

typedef int kilometer; // like an alias

int main() {
  kilometer newywork, chicago, miami;

  newywork = 300;
  chicago = 200;
  miami = 1900;

  printf("Distance newyork = %d chicago = %d miami = %d\n", newywork, chicago,
         miami);
}
