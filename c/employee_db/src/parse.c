
#include "parse.h"
#include "common.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>

void read_employees_list(dbheader_t *dbhdr, employee_t *employees) {
    for (int i = 0; i < dbhdr->count; i++) {
        printf("Employee: %d\n",i);
        printf("\tName=%s Address=%s Hours=%d\n",employees[i].name,employees[i].address,employees[i].hours);
    }
}

int add_employee(dbheader_t *dbhdr, employee_t *employees, char *add_string) {
    char *name = strtok(add_string, ",");
    char *addr = strtok(NULL, ",");  // on second call don't pass anything, internal vairiable keeps track
    char *hours = strtok(NULL,",");

    // we do dbhdr->count -1 because count is a count not and index
    strncpy(employees[dbhdr->count -1].name,name,sizeof(employees[dbhdr->count-1].name));
    strncpy(employees[dbhdr->count -1].address,addr,sizeof(employees[dbhdr->count-1].address));

    employees[dbhdr->count-1].hours = atoi(hours);

    printf("name=%s | address=%s | hours=%s \n",name,addr,hours);
    return STATUS_SUCCESS;
}

int create_db_header(int fd,dbheader_t **headerOut){

// since we want the caller to be able to use it, we do not want it in stgack
  dbheader_t *header = calloc(1, sizeof(dbheader_t));  // sets all fields to 0

  if (header == NULL) {
      perror("allocation");
      printf("calloc failed for create db header \n");
    return STATUS_ERROR;
  }
  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(dbheader_t);

  // when headerOut is derefernece it will point to header
  *headerOut = header;
  return STATUS_SUCCESS;
}  // headerOut will be in the in memory version of db



int validate_db_header(int fd,dbheader_t **headerOut) {
    if (fd < 0) {
        printf("validate_db_header() got invalid fd from user\n");
        return STATUS_ERROR;
    }


    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == NULL) {
        perror("alloc");
        printf("validate_db_header(): unable to calloc\n");
        return STATUS_ERROR;
    }

    // When reading, it should be in network endianness (bigindian)
    if (read(fd, header, sizeof(dbheader_t)) != sizeof(dbheader_t)) {
        perror("reading header");
        free(header);
        return STATUS_ERROR;
    }
    // we should now unpack it in host indianness
    header->version = ntohs(header->version);  // network to host short
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);


    if (header->version != 1) {
        printf("expected db header version=1 got=%d\n",header->version);
        free(header);
        return STATUS_ERROR;
    }

    if (header->magic != HEADER_MAGIC) {
        printf("expected db header magic=0x%x got=0x%x\n",HEADER_MAGIC,header->magic);
        free(header);
        return STATUS_ERROR;
    }

    if (header->version != 1) {
        printf("expected db header version=1 got=%d\n",header->version);
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("expected db filesize=%ld got=%d\n",dbstat.st_size,header->filesize);
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}


int output_file(int fd, dbheader_t *dbhdr,employee_t *employees) {
    if (fd < 0) {
        printf("invalud fd for output_file\n");
        return STATUS_ERROR;
    }

    // preserving real count for the loop before converting to network endian
    int real_count = dbhdr->count;
    // we alway want to save it network endianess
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(dbheader_t) + sizeof(employee_t) * real_count);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    if (lseek(fd,0,SEEK_SET) == -1) { // SEEK_SET is to be set with absolute offset not relative to current position\

        printf("current fd %d\n",fd);
        perror("output db header");
        return STATUS_ERROR;
    }

    if (write(fd,dbhdr,sizeof(dbheader_t)) == -1) {
        perror("db header write");
        return STATUS_ERROR;
    }

    for (int i = 0; i < real_count; i++) {
        employees[i].hours = htons(employees[i].hours);

        int ret = write(fd,&employees[i],sizeof(employee_t));
        if (ret == -1) {
            perror("write");
            return STATUS_ERROR;
        }
    }
    return STATUS_SUCCESS;
}



int read_employees(int fd, dbheader_t *dbhdr,employee_t **employee_out) {
    if (fd == -1 ) {
        printf("invalid file descriptor for read_employees\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;
    // we can baiscally treat it as an array now, count is the number of employees we have
    employee_t *employees = calloc(count,sizeof(employee_t));
    if (employees == NULL) {
        perror("allocation failed during read_employees");
        return STATUS_ERROR;
    }


    // we are assuming that offset has been successfully forwarded here
    if (read(fd,employees,count * sizeof(employee_t)) == -1) {
        perror("while reading employee");
        return STATUS_ERROR;
    }

    for (int i = 0; i<count; i++) {
        employees[i].hours = ntohs(employees[i].hours);
    }

    *employee_out = employees;

    //int cur = lseek(fd, 0, SEEK_CUR);
    //printf("current seek %d\n",cur);
    return STATUS_SUCCESS;
}
