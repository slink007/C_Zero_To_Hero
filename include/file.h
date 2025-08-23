#ifndef FILE_H
#define FILE_H

/* New version, as imposed by online grader. Attempt to create a new file whose 
 * name is stored in 'fileneme'. Returns file descriptor on success, or else 
 * returns -1 on failure. */
int create_db_file(char *filename);

/* Attempt to open a file whose name is stored in 'fileneme'. The file
 * descriptor for this file is stored in 'fd'. Returns STATUS_SUCCESS or 
 * STATUS_ERROR.*/
int open_db_file(char *filename, int *fd);

/* Reads data from file associated with file descriptor 'fd'. Uses data within the
 * dbhdr to set aside enough memory to hold the file's contents.  The employeesOut input
 * refers back to an array of structs which will hold the file contents. */
int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut);

/* Attempts to write the structs contained within 'employees' out to the file referenced by
 * file descriptor 'fd'.  The 'dbhdr' is needed because its contents must be the first line
 * written to the file.  Returns STATUS_ERROR if 'fd' is bad, but otherwise returns 
 * STATUS_SUCCESS after writing to the file. */
int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees);

/* Attempt to open a file whose name is stored in 'fileneme'.  The file
 * descriptor for this file is stored in 'fd'.  The file is openened in write mode
 * to ensure that all prior contents are erased.  Returns STATUS_SUCCESS if the file
 * was opened or STATUS_ERROR if it was not.*/
int truncate_db_file(char *filename, int *fd);

#endif
