#ifndef PARSE_H
#define PARSE_H

// "LLAD" for Low Level Academy Database
#define HEADER_MAGIC 0x4c4c4144


int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring);

#endif
