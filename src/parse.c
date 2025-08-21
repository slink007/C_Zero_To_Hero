#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "common.h"
#include "parse.h"


int validate_db_header(int fd, struct dbheader_t **headerOut) {
	// validate file descriptor
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	// create storage for a header
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed create a db header\n");
		return STATUS_ERROR;
	}

	// read header info from file into created storage
	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	/* Adapt values from the database file (which could reside on another system) 
	   to the format used on this system. */
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("Impromper header magic\n");
		free(header);
		return STATUS_ERROR;
	}

	if (header->version != 1) {
		printf("Impromper header version\n");
		free(header);
		return STATUS_ERROR;
	}

	// Is DB the correct size?
	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database. Header shows %d bytes while fstat indicates %ld bytes.\n", header->filesize, dbstat.st_size);
		printf("An entry is %ld bytes.\n", sizeof(struct employee_t));
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;
	return STATUS_SUCCESS;
}


int create_db_header(struct dbheader_t **headerOut) {
	/* The idea for this part is that while headerOut is pointing to NULL,
	   headerOut itself still exists.  At least under normal conditions.
	   If something goes wrong and NULL is passed in then a segfault 
	   occurs when attempting to assign this function's allocated memory
	   to NULL.
   */
	if (headerOut == NULL) {
		return STATUS_ERROR;
	}
	
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) { 
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;  // Assigns allocated memory to the pointer passed into this function.

	return STATUS_SUCCESS;
}


void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	for (int i = 0; i < dbhdr->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}
}


int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	// Handle the case where function is called without employee information being passed in.
	if ( addstring == NULL ) {
		return STATUS_ERROR;
	}
	
	char *name = strtok(addstring, ",");
	char *address = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");
	
	strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
	strncpy(employees[dbhdr->count-1].address, address, sizeof(employees[dbhdr->count-1].address));
	employees[dbhdr->count-1].hours = atoi(hours);
	
	return STATUS_SUCCESS;
}


int change_hours(struct dbheader_t *dbhdr, struct employee_t *employees, char *newHours) {
	char *name = strtok(newHours, ",");
	char *hours = strtok(NULL, ",");

	int i = 0;
	for(; i < dbhdr->count; i++) {
		if ( strcmp(employees[i].name, name) == 0 ) {
			employees[i].hours = atoi(hours);
			return STATUS_SUCCESS;
		}
	}
	return STATUS_ERROR;
}


int remove_employee(char *name, struct dbheader_t *dbhdr, struct employee_t **employees) {
    int status = NAME_NOT_FOUND;
    int copy_limit = dbhdr->count - 1;
	int employees_read = 0;

	struct employee_t *copy_to = NULL;
	copy_to = *employees;
	struct employee_t *copy_from = NULL;
	copy_from = *employees;

	while (employees_read < dbhdr->count) {
		if (strcmp((*copy_from).name, name) != 0) {  // this is not the person to remove
			memcpy(copy_to, copy_from, sizeof(struct employee_t));
			copy_to++;
		} else {
			status = NAME_FOUND;
		}
		copy_from++;
		employees_read++;
	}

	if (status == NAME_FOUND) {
		struct employee_t *new_employees = (struct employee_t *)realloc(*employees, copy_limit * sizeof(struct employee_t));
		if (new_employees == NULL) {
			perror("Realloc failed");
			free(*employees);
			return STATUS_ERROR;
		} 
		*employees = new_employees;
		dbhdr->count = copy_limit;
		return STATUS_SUCCESS;
	} else {
		free(*employees);
		return STATUS_ERROR;
	}
}
