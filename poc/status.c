#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

int main(int argc, char *argv[]) {
	char *s = strdup("HTTP/1.1 200 OK\r\na: b\r\nc: d\r\n");
	char *version=NULL, *status=NULL, *message=NULL, *p;
	int ret = 0;
	if (p=strstr(s, "\r\n")) {
		*p = 0;
		p += 2;
		if (!(version=strtok(s, "/")) || strcmp(version, "HTTP"))
			ret = 3; //wrongly unspecified protocol
		else if ((version=strtok(NULL, " ")) &&
				(status=strtok(NULL, " ")) &&
				(message=strtok(NULL, "\r"))) {
			printf("version is %s\n", version);
			printf("status is %s\n", status);
			printf("message is %s\n", message);
			printf("%s\n", p);
		}
		else
			ret = 2; //invalid syntax
	}
	else
		ret = 1; //no status line
end:
	free(s);
	return ret;
}
