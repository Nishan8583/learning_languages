#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x6e697368

typedef  struct {
  unsigned int magic; // this is the magic header
  unsigned short version; // version of db
  unsigned short count;
  unsigned int filesize;
} dbheader_t ;

typedef struct {
  char name[256];
  char address[256];
  unsigned int hours;
} employee_t;


void read_employees_list(dbheader_t *dbhdr, employee_t *employees);
int create_db_header(int fd,dbheader_t **headerOut);  // headerOut will be in the in memory version of db
int validate_db_header(int fd,dbheader_t **headerOut);
int read_employees(int fd, dbheader_t *,employee_t **employee_t);
int output_file(int fd, dbheader_t *header, employee_t *employees);
int add_employee(dbheader_t *dbhdr, employee_t *employees, char *add_string);
#endif
