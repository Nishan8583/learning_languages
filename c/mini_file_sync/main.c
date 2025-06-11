#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "copy_file.h"

#define LIMIT 512

char *DEST = "./dest/";

int main() {
  DIR *d= opendir("./src");
  
  if (d == NULL) {
    perror("open directory failed");
    return -1;
  }

  struct dirent *entry = {0};

  while ((entry = readdir(d)) != NULL) {

    // if this is not a regular file just continue
    if (entry->d_type != DT_REG) {
      continue;
    }

    // holds file name
    char path[LIMIT];

    int err = snprintf(path, LIMIT, "./src/%s",entry->d_name);
    if (err < 0) {
      printf("snprintf failed with error=%d\n",err);
      return err;
    }

    // safe initialization to 0
    struct stat statbuf = {0};  // this will be in the stack,  data from previous iteration will be overwritten in stat() call

    if (stat(path,&statbuf) == -1) {
      perror("stat(): failed");
      return -1;
    }

    printf("Filename=%s Size=%ld last modified=%ld\n",path,statbuf.st_size,statbuf.st_mtime);
    
    // craft destination path
    char dest[LIMIT] = {0};
    int err2 = snprintf(dest,LIMIT,"./dest/%s",entry->d_name);
    if (err2 < 0) {
      printf("main(): unable to snprintf on destination file due to error=%d\n",err2);
      return err2;
    }

    if (copy_file(path,dest) == -1)  {
      printf("main(): error returned from copy_file()");
      return -1;
    }


  }

}
