#ifndef COMMON_H
#define COMMON_H

#define STATUS_ERROR   -1
#define STATUS_SUCCESS 0

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



#endif
