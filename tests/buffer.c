#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "http_headers.h"
#include "http.h"
#include "buffer.h"

void dumb_print(char *buffer, size_t size) {
	write(1, "*", 1);
	write(1, buffer, size);
	fprintf(stderr, "size=%d\n", size); //FIXME
}

int main(int argc, char *argv[]) {
	int fd = get_tcp_socket("localhost", "2042", 0);
	if (fd == -2)
		return 1;
	else if (fd == -1) {
		perror("get_tcp_socket");
		return 2;
	}
	else {
		BUFFER *buf = NULL;
		char *s;
		size_t r = read_until(fd, &s, "Pouet", 1024, &buf);
		if (r == -1)
			perror("read_until");
		else {
			s[r] = 0;
			printf("*%s", s);
			free(s);
			//once again
			r = read_until(fd, &s, "pa", 1024, &buf);
			if (r == -1)
				perror("read_until'");
			else {
				s[r] = 0;
				printf("*%s", s);
				fflush(stdout);
				read_all(fd, &buf, 1024, dumb_print);
				free(s);
			}
		}
		close(fd);
		return 0;
	}
}
