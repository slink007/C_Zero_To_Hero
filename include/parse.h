#ifndef PARSE_H
#define PARSE_H

// "LLAD" for Low Level Academy Database
#define HEADER_MAGIC 0x4c4c4144

struct dbheader_t {
	unsigned int magic;      // tells the file parser if this is a valid DB file or not
	unsigned short version;  // shows the version of the DB file
	unsigned short count;    // number of records in the file
	unsigned int filesize;   
};

struct employee_t {
	char name[128];
	char address[128];
	unsigned int hours;
};

int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut);
int output_file(int fd, struct dbheader_t *, struct employee_t *employees);
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring);

#endif
