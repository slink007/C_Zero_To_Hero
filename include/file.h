#ifndef FILE_H
#define FILE_H

/* Attempt to create a new file whose name is stored in 'fileneme'. The file
   descriptor for this file is stored in 'fd'. Returns STATUS_SUCCESS or 
   STATUS_ERROR.*/
int create_db_file(char *filename, int *fd);


/* Attempt to open a file whose name is stored in 'fileneme'. The file
   descriptor for this file is stored in 'fd'. Returns STATUS_SUCCESS or 
   STATUS_ERROR.*/
int open_db_file(char *filename, int *fd);

int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut);
int output_file(int fd, struct dbheader_t *, struct employee_t *employees);

#endif
