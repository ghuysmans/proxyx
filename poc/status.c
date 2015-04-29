#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "http_headers.h"
#include "http.h"

int main(int argc, char *argv[]) {
	int fd = open("a.txt", O_RDONLY);
	char buf[100], *fl, *d;
	HTTP_HEADER *h;
	size_t dl, rem;
	int ret;

	if (fd == -1) {
		perror("open");
		return 1;
	}
	else if ((ret=fetch_http(fd, buf, sizeof(buf), &fl, &h, &d, &dl, &rem)) == -1)
		return -1;
	else {
		HTTP_HEADER *p;
		printf("headers:\n");
		for (p=h; p; p=p->next)
			printf("%s=%s\n", p->field_name, p->field_value);
		printf("data:\n%s", d);
		free_http_headers(h);
		free(d);
		return 0;
	}
}
