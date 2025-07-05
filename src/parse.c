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


/* Not sure how I wound up with returning an int, because in the videos the return type
 * is void, and nothing is returned.  Need to fix? */
int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;

	// It's possible that the file is on some system other than the one the database is running on,
	// and that other system may have different "endianess" than this system has. htonl (Host To
	// Network Long) and htons (Host To Network Short) make the appropriate conversions.
	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	// Ensure that content goes to the very beginning of the file.
	lseek(fd, 0, SEEK_SET);

	// Write the header
	write(fd, dbhdr, sizeof(struct dbheader_t));

	int i = 0;
	for (; i < realcount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

	return STATUS_SUCCESS;
	/* When testing with an entry of "Timmy H.,123 Sheshire Ln.,120" the value is correctly
	 * added to the db file, but it may not appear so at first. We write raw bytes to the file,
	 * so when strings are sent they are in ASCII format and look like one expects when the file
	 * is printed. The number, 120, is stored as 120 or 0x78. This turns out to be ASCII for
	 * the letter 'x', and that's what you see when examining the file. */
}	


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
	int i = 0;
	for (; i < dbhdr->count; i++) {
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

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}
	
	int count = dbhdr->count;
	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == NULL) { 
		printf("Malloc failed\n");
		return STATUS_ERROR;
	}
	read(fd, employees, count*sizeof(struct employee_t));
	
	/* This is interesting. Apparently, by declaring employees as a pointer to 'count' blocks
	 * of memory, we have implicitly created an array, and we can use subscripts on it. */
	int i = 0;
	for(; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}
	
	*employeesOut = employees;
	return STATUS_SUCCESS;
}



