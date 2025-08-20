#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"


int print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n - create new database file\n");
	printf("\t -f - (required) path to database file\n");
	printf("\t -a <name,address,hours> adds a new employee\n");
	printf("\t -h <name,hours> changes hours of 'employee' to 'hours'");
	return 0;
}

int main(int argc, char *argv[]) { 
	char *filepath = NULL;
	char *addstring = NULL;
	char *newHours = NULL;
	char *removeName = NULL;
	bool newfile = false;
	bool list = false;
	int c = 0;
	int dbfd = -1;  // database file descriptor
	struct dbheader_t *dbhdr = NULL;
	struct employee_t *employees = NULL;

	// option "n" expects no input because it has no modifier
	// option "f:" expects input due to the colon modifier
	while ((c = getopt(argc, argv, "nf:a:lh:r:")) != -1) {
		switch(c) {
			case 'f':
				filepath = optarg;
				break;
			case 'n':
				newfile = true;
				break;
			case 'l':
				list = true;
				break;
			case 'a':
				addstring = optarg;
				break;
			case 'h':
				newHours = optarg;
				break;
			case 'r':
				removeName = optarg;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				print_usage(argv);
				break;
			default:
				printf("We shouldn't be here...\n");
				print_usage(argv);
				return -1;
		}
	}
	
	if (filepath == NULL) {
		printf("Filepath is a required argument.\n");
		print_usage(argv);
	}
	
	if (newfile) {
		dbfd = create_db_file(filepath);
		if ( dbfd == STATUS_ERROR ) {
			if ( filepath == NULL) {
				printf("Filepath is a required argument.\n");
			} else {
				printf("Unable to create database file %s\n", filepath);
			}
			return STATUS_ERROR;
		}
		
		if ( create_db_header(&dbhdr) == STATUS_ERROR ) {
			printf("Failed to create database header\n");
			return STATUS_ERROR;
		}
	} else {
		if ( open_db_file(filepath, &dbfd) == STATUS_ERROR ) {
			printf("Unable to open database file %s\n", filepath);
			return STATUS_ERROR;
		}
		
		if ( validate_db_header(dbfd, &dbhdr) == STATUS_ERROR ) {
			printf("Failed to validate database header\n");
			return STATUS_ERROR;
		}
	}
	
	// Read existing contents from db file.
	if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
		printf("Failed to read employees\n");
		return 0;
	}
	
	// Add an employee
	if (addstring) {
		dbhdr->count++;
		employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
		add_employee(dbhdr, employees, addstring);
	}
	
	// List out all employees in database
	if (list) {
		list_employees(dbhdr, employees);
	}

	// Alter an employee's hours
	if (newHours) {
		change_hours(dbhdr, employees, newHours);
	}

	// Remove someone from database
	if (removeName) {
		if (remove_employee(removeName, dbhdr, &employees) == STATUS_SUCCESS) {
			printf("Successfully removed %s from the database.  Database now contains %d entries.\n", removeName, dbhdr->count);
		} else {
			printf("Unable to remove %s from the database.\n", removeName);
			exit(1);
		}
	}
	
	/* Need to
     * 1. close the dbfd file    O_WRONLY | O_TRUNC, 0644
     * 2. open it in write mode (forces all content to be removed)
     * 3. remove ftrunctate() from output_file
     */
    close(dbfd);
    if ( truncate_db_file(filepath, &dbfd) == STATUS_ERROR ) {
		printf("Unable to open database file %s\n", filepath);
		return STATUS_ERROR;
	} else {
        output_file(dbfd, dbhdr, employees);
    }
	
	return STATUS_SUCCESS;	
}
