#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "copy_file.h"
#include "file_sync.h"

int main() {
  char *src = "./src";
  char *dst = "./dest";

  file_sync(src,dst);
}
