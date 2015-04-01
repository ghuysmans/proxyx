#include "socket.h"
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define EASYGET "GET / HTTP/1.1\r\n" \
	"Connection: close\r\n" \
	"Host: www.google.fr\r\n\r\n"

int main(int argc, char *argv[]) {
	char *prot="http";
	int o, verbose=0;
	//TODO use a port of this library on Windows
	while ((o=getopt(argc, argv, "p:v")) != -1) {
		switch (o) {
			case 'p':
				prot = optarg;
				break;
			case 'v':
				verbose++;
				//break;
		}
	}
	if (argv[optind]) {
		int sock;
		ssize_t r;
		char buf[1024];
		if ((sock=get_tcp_socket(argv[optind], prot, 0)) == -1)
			return 1;
		if (send(sock, EASYGET, strlen(EASYGET), 0) == -1) {
			perror("send");
			return errno;
		}
		while ((r = recv(sock, buf, sizeof(buf)-1, 0))!=0 && r!=-1) {
			buf[r] = 0;
			puts(buf);
		}
		if (r == -1)
			perror("recv");
		close(sock);
	}
	else
		printf("Usage: %s name\n", argv[0]);
	return 0;
}
