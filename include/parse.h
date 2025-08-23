#ifndef PARSE_H
#define PARSE_H

// "LLAD" for Low Level Academy Database
#define HEADER_MAGIC 0x4c4c4144
#define NAME_FOUND 1
#define NAME_NOT_FOUND 0


/* Attempts to create a header for a new database file.  Returns STATUS_SUCCESS if this is
   successful, or STATUS_ERROR if it is not. */
int create_db_header(struct dbheader_t **headerOut);


/* Attempts to read the database header from the file and ensure that it is properly formed.
   Returns STATUS_SUCCESS if the header is validated.  Returns STATUS_ERROR if the database
   file cannnot be opened or if the header fails any of the verification tests. */
int validate_db_header(int fd, struct dbheader_t **headerOut);


/* Prints an ID number and all information for each employee in the database. */
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);


/* Add an employee to the database.  Currently returns STATUS_SUCCESS no matter what.
   Needs an update to either return STATUS_ERROR under certain conditions or else just
   make it return void. */
/* To satisfy the grader, this function needs to be rewritten to use 
 * int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring);
 */
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring);


/* Searches database using an employee's name and revises their worked hours. Returns
   STATUS_SUCCESS if the change is made, or STATUS_ERROR if the change is not made. */
int change_hours(struct dbheader_t *dbhdr, struct employee_t *employees, char *newHours);

/* Searches database using an employee's name.  If a match is made then the employee is
   removed from the database and STATUS_SUCCESS is returned.  If no match is made then
   STATUS_ERROR is returned. */
int remove_employee(char *name, struct dbheader_t *dbhdr, struct employee_t **employees);

#endif
