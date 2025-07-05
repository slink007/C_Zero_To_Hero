#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"


int test_fd(int fd) {
	if (fd == -1) {
		perror("open");
		close(fd);
		return STATUS_ERROR;
	}
	return fd;
}


int create_db_file(char *filename) {
	int fd = open(filename, O_RDWR | O_CREAT, 0644);
	return test_fd(fd);
}


int open_db_file(char *filename) {
	int fd = open(filename, O_RDWR, 0644);
	return test_fd(fd);
}

