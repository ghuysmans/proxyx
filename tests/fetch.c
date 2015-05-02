#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "http.h"
#include "socket.h"

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int fd = open(argv[1], O_RDONLY);
		BUFFER *b = NULL;
		char *fl;
		HTTP_HEADER *h;
		char *d; size_t dl;
		size_t rem;
		int ret;
		if (fd == -1) {
			perror("open");
			return 1;
		}
		else if ((ret=fetch_http(fd, &b, 1024, &fl, &h, &d, &dl, &rem)) == -1)
			return -1;
		else {
			HTTP_HEADER *p;
			printf("status: %s\n", fl);
			printf("headers:\n");
			for (p=h; p; p=p->next)
				printf("%s=%s\n", p->field_name, p->field_value);
			printf("data:\n%s", d);
			free_http_headers(h);
			free(d);
			return 0;
		}
	}
	else
		printf("usage: %s input\n", argv[0]);
}
