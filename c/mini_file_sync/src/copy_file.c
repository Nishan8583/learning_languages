
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#define DEST "./dest/"

int copy_file(char *src, char *dst) {
  DIR *d= opendir(DEST);

  // if directory exists
  if (d) {
    closedir(d);
  } else if (ENOENT == errno) {
    printf("copy_file(): %s directory does not exist, attempting to create it.\n",DEST);
    // if the errno vairable was set to this, means the directory does not exist 
    if (mkdir(DEST,0700) == -1) {
      perror("copy_file(): could not create directory");
      return -1;
    }
    printf("copy_file(): %s directory successfully created\n",DEST);
  } else {
    perror("copy_file(): opendir erro");
    return -1;
  }

  int src_fd = open(src,O_RDONLY);
  int dst_fd = open(dst,O_CREAT | O_WRONLY,0644);
  if (src_fd == -1) {
    printf("copy_file(): on source file open %s\n",src);
    perror("open file");
    if (dst_fd != -1) {
      close(dst_fd);
    }
    return src_fd;
  }

  if (dst_fd == -1) {
    printf("copy_file(): on destination file open %s\n",dst);
    perror("open file");
    if (src_fd != -1) {
      close(src_fd);
    }
    return dst_fd;
  }

  char buffer[4096] = {0};

  while (1) {
    int n = read(src_fd,buffer,4096);
    if (n == -1) {
      perror("copy_file(): while reading source file");
      return n;
    }

    if (n == 0) {
      printf("copy_file(): Done reading for %s\n",src);
      break;
    }

    // ignore when some bytes are not written for now
    if (write(dst_fd,buffer,4096) == -1) {
      perror("copy_file(): when writing to destination file");
      return -1;
    }
  }
  close(src_fd);
  close(dst_fd);



  printf("copy_file(): Success for source=%s and destination=%s\n",src,dst);
  return 0;

}
