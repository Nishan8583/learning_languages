#include "fs.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc > 1) {
    printf("|%s|\n", argv[1]);
    if (strcmp(argv[1], "--test") == 0) {
      printf("test found\n");
      printf("|%s|\n", argv[2]);
      if (argc < 3) {
        test_read_and_load_fs(NON_DEBUG_MODE);
      } else if (strcmp(argv[2], "--debug") == 0) {
        test_read_and_load_fs(DEBUG_MODE);
      }
    }
  }
}
