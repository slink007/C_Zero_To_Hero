#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"


	


/* This should be called something else? Create header?
   Seems like a lot more than validation is going on here. */
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

	// The "ntohs" or "Network To Host Short" call is a Linux utility which 
	// converts data to the proper "endianess" for the system. So we read 
	// stuff from the header, convert it to the proper format for our system, 
	// and write it back to memory.
	//
	// "ntohl" is doing essentially the same thing, it's just returning a long int
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("Impromper header magic\n");
		free(header);
		return -1;
	}

	if (header->version != 1) {
		printf("Impromper header version\n");
		free(header);
		return -1;
	}

	// Is DB the correct size?
	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database\n");
		free(header);
		return -1;
	}

	*headerOut = header;
}


int create_db_header(int fd, struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) { 
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;    // Assigns allocated memory to the pointer passed into this function.

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
	char *name = strtok(addstring, ",");
	char *address = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");
	
	strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
	strncpy(employees[dbhdr->count-1].address, address, sizeof(employees[dbhdr->count-1].address));
	employees[dbhdr->count-1].hours = atoi(hours);
	
	return STATUS_SUCCESS;
}


