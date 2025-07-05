#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

/* TODO
 * 1.  Add ability to remove an employee (use their name to find them).
 * 2.  Add ability to change an employee's hours (use their name to find them).
 */

int print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n - create new database file\n");
	printf("\t -f - (required) path to database file\n");
	return 0;
}

int main(int argc, char *argv[]) { 
	char *filepath = NULL;
	char *addstring = NULL;
	bool newfile = false;
	bool list = false;
	int c = 0;
	int dbfd = -1;  // database file descriptor
	// int status = STATUS_ERROR;
	struct dbheader_t *dbhdr = NULL;
	struct employee_t *employees = NULL;

	// option "n" is a binary option because it has no modifier
	// option "f:" is a string due to the colon modifier
	while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
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
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				printf("We shouldn't be here...\n");
				return -1;
		}
	}
	
	if (filepath == NULL) {
		printf("Filepath is a required argument.\n");
		print_usage(argv);
	}
	
	if (newfile) {
		if ( create_db_file(filepath, &dbfd) == STATUS_ERROR ) {
			printf("Unable to create database file %s\n", filepath);
			return STATUS_ERROR;
		}
		
		if ( create_db_header(dbfd, &dbhdr) == STATUS_ERROR ) {
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
	
	if (addstring) {
		dbhdr->count++;
		employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
		add_employee(dbhdr, employees, addstring);
	}
	
	if (list) {
		list_employees(dbhdr, employees);
	}
	
	output_file(dbfd, dbhdr, employees);
	
	return 0;	
}
