#include <stdio.h>
#define LENGTH                                                                 \
  10000 // preprocessor goes through the code, and replace the value LENGTH with
        // value 10000

int main() {
  printf("%d\n", LENGTH);
  const int length = 10; // like any other variable, we just can not change it
  for (int i = 0; i < length; i++) {
    printf("%d\n", i);
  }
}
