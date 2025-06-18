
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "copy_file.h"

#define LIMIT 512

#define ERR_SRC_DST_NULL -1

char *DEST = "./dest/";

int file_sync(char *src, char *dst) {

    // check if null
    if (src == NULL || dst == NULL) {
        printf("src or dst is null, please a pointer to string src=%s dst=%s\n",src,dst);
        return ERR_SRC_DST_NULL;
    }

    // open the directory
    DIR *d= opendir(src);
    if (d == NULL) {
        perror("open directory failed");
        return -1;
    }

    struct dirent *entry = {0};

    // read each files in the directory
    while ((entry = readdir(d)) != NULL) {

        // if this is not a regular file just continue
        if (entry->d_type != DT_REG) {
            continue;
        }

        // holds file name
        char path[LIMIT];

        int err = snprintf(path, LIMIT, "%s/%s",src,entry->d_name);
        if (err < 0) {
            printf("snprintf failed with error=%d\n",err);
            return err;
        }

        // safe initialization to 0
        struct stat statbuf = {0};  // this will be in the stack,  data from previous iteration will be overwritten in stat() call

        err = stat(path,&statbuf);
        if (err < 0){
            perror("stat(): failed");
            return err;
        }

        printf("Filename=%s Size=%ld last modified=%ld\n",path,statbuf.st_size,statbuf.st_mtime);
        
        // craft destination path
        char dest[LIMIT] = {0};
        int err2 = snprintf(dest,LIMIT,"%s/%s",dst,entry->d_name);

        if (err2 < 0) {
            printf("main(): unable to snprintf on destination file due to error=%d\n",err2);
            return err2;
        }

        struct stat destStatBuf = {0};
        int updateFile = 0;
        if (stat(dest,&destStatBuf) == -1 && errno != ENOENT) {
            updateFile = 1;
        } else if (statbuf.st_size != destStatBuf.st_size) {  // size does not match need to update it
            updateFile = 1;
        } else if (statbuf.st_mtime > destStatBuf.st_mtime) {
            updateFile = 1;
        }


        // if any of the update file conditons were matched, then copyfile
        if (updateFile) {
            if (copy_file(path,dest) == -1)  {
                printf("main(): error returned from copy_file()");
                return -1;
            }
        }


    }

    return 0;

}