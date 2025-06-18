#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "file_sync.h"

char *src = "./src_test";
char *dest = "./dest_test";

char *test_file_1 = "./src_test/test_file1";
char *test_file_2 = "./src_test/test_file2";
char *test_file_3 = "./src_test/test_file3";

char *test_file_4 = "./src_test/test_file4";
char *test_file_5 = "./src_test/test_file5";
char *test_file_6 = "./src_test/test_file6";

// char *char_files[] = {test_file_1, test_file_2, test_file_3, test_file_4,
//                      test_file_5, test_file_6, NULL};  // this will fail
//                      because it wants a compile time constat, and that means
//                      point to somehting is not? it wants string literals

char *test_files[] = {
    "./src_test/test_file1",
    "./src_test/test_file2",
    "./src_test/test_file3",
    "./src_test/test_file4",
    "./src_test/test_file5",
    "./src_test/test_file6",
    NULL,
};

char *dest_test_files[] = {
    "./dest_test/test_file1",
    "./dest_test/test_file2",
    "./dest_test/test_file3",
    "./dest_test/test_file4",
    "./dest_test/test_file5",
    "./dest_test/test_file6",
    NULL,
};

void prepare_directories_for_test() {

  struct stat src_stat = {0};
  struct stat dest_stat = {0};

  // if src file does not exist create one
  if (stat(src, &src_stat) == -1) {
    if (mkdir(src, 0700) == -1) {
      perror("prepare_directories_for_test(): could not create src directory "
             "for test");
      exit(-1);
    }
  }

  if (stat(dest, &dest_stat) == -1) {
    if (mkdir(dest, 0700) == -1) {
      perror("prepare_directories_for_test(): could not create dest directory "
             "for test");
      exit(-1);
    }
  }
}

void cleanup() {
  for (int i = 0; test_files[i] != NULL; i++) {
    int err = unlink(test_files[i]);
    if (err != 0) {
      perror("cleanup(): unlinking test files failed");
    }
  }

  for (int i = 0; dest_test_files[i] != NULL; i++) {
    int err = unlink(dest_test_files[i]);
    if (err != 0) {
      perror("cleanup(): unlinking test files failed");
    }
  }

  if (rmdir(src) == -1 || rmdir(dest) == -1) {
    perror("unable to remove test directory");
  }
}

// Test Case 1: do a normal copy and then if file exists
int TestCase1() {
  prepare_directories_for_test();

  // check if files does not exist, if it does not, then create them
  for (int i = 0; test_files[i] != NULL; i++) {
    if (access(test_files[i], F_OK) !=
        0) { // this was the easiest way to check them
      int fd = open(test_files[i], O_CREAT | O_WRONLY);
      if (fd == -1) {
        perror("TestCase1(): could not create file");
        cleanup();
        return fd;
      }

      close(fd);
    }
  }

  if (file_sync(src, dest) != 0) {
    printf("TestCase1(): failed\n");
    cleanup();
    return -1;
  }

  printf("TestCase1(): Success\n");
  cleanup();
  return 0;
}

int main() { return TestCase1(); }
