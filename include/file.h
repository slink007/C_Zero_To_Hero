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

#endif
